//TODO
//1. in finishSequence we can't get under granularity of hardcoded divider, for JS that's 1s at the moment - hence we don't know which genes are best once we are below that time!

function b(i) { if(typeof i=='string') return parseInt(i,2); throw "Expects string"; } //for byte defs

var MOVE_TIMEOUT            = 300; //timeout of a move in ms
var FIRST_GENERATION_TIMOUT = 600000;   //timout of a series for first generation, in ms
var TIMEOUT_MULTIPLIER     = 50;  //since only 5 bytes are used for move time, we multiply it by this number of MS

var SEED_GENOME        = false; //set to true reseed genome... otherwise whatever is in EPROM is used
var SIM_REPOPULATE     = false;
var DONT_MOVE          = false;//true only for debug

//genes
//first 3 bytes are commands
//last 5 bytes are duration
var G_NOP       =b("00000000"); // no operation
//                  0b00100000
var G_LEFT      =b("01000000");
var G_RIGHT     =b("01100000");
var G_FORWARDS  =b("10000000");
var G_BACKWARDS =b("10100000");
var G_REVERT    =b("11000000");
//#define G_PAUSE     0b11100000 //not implemented
var G_MASK      =b("11100000");
var T_MASK      =b("00011111");
//define subcommands / speed
var TIL_HIT     =b("00011111");  //move until hit


var GENOME_LENGTH = 20;
var POPULATION_SIZE =10;
//genome structure, bytes:
  //b0                 = generation
  //b1                 = health
  //b2                 = success
  //b3                 = command for bump interriupt
  //b4-bGENOME_LENGTH  = algorythm
var GENOME_START =4; //array index of genome start

//states
var STARTING        =0;
var NEXT_SEQUENCE   =1;
var NEXT_LOOP       =2;
var THINKING        =3;
var MOVING          =4;
var STALLED         =5;
var DONE_MOVE       =6;
var REPOPULATING    =7;
var SLEEP           =8;
var GENERATION_DONE =252;
var SEQUENCE_DONE   =253;
var DEAD            =254;
var FREE            =255;

//some constants for eeprom locations
var EEPROM_CURRENT_GENOMEID_ADDR     = 1023;
var EEPROM_CURRENT_GENERATIONID_ADDR = 1022;
var EEPROM_GENERATION_BEST_TIME = 1021;
var EEPROM_GENERATION_TIMEOUT=1020;

//TODO?
var freedom=false; //global variable as we can't use class member function


function FreedomController(arbotPlatform){
  this.state                    = STARTING;
  this.moveTimeout              = 0;
  this.startedMovingTimestamp   = 0;
  this.currentGenome            = 0;
  this.currentSequence          = 0;
  this.sequenceStartTimestamp   = 0;
  this.currentStep              = GENOME_START;
  this.interruptsDisabled       = false; //disable interrupts while in interrupt handler
  this.lastCommand              = 0;     //we remember previous command to support revert command
  this.lastTime                 = 0;     //same for time
  this.arbotPlatform            = arbotPlatform;
  this.generationTotalTime      = 0;
  this.generationFreeCount      = 0;

  this.loop=function(){
    if(SEED_GENOME) {return;} //don't do processing while seeding
    //console.log("current state:");
    //console.log(this.state);
    switch(this.state){
    case STARTING:
      this.state=NEXT_SEQUENCE;
      break;
    case NEXT_SEQUENCE:
      this.initSequence();
      break;
    case NEXT_LOOP:
      this.initLoop();
      break;
    case DONE_MOVE:
      this.think();
      break;
    case THINKING:
      this.move(this.lastCommand,this.lastTime);
      break;
    case MOVING:
      this.checkFreedom();
      break;
    case STALLED:
      this.interruptsDisabled?this.setStateDoneMove():this.stallInterrupt();
      break;
    case FREE:
      this.finishSequence();
      break;
    case DEAD:
      this.finishSequence();
      break;
    case SEQUENCE_DONE:
      this.switchGenome();
      break;
    case GENERATION_DONE:
      this.repopulate();
      break;
    case SLEEP:
      break;
    }
  };


  this.initSequence=function(){
    arbotPlatform.reset();//js only to move player back to beginning
    this.currentGenome=this.getCurrentGenomeId();
    this.currentStep=GENOME_START;
    this.sequenceStartTimestamp=millis();
    this.state=NEXT_LOOP;
    this.freedom=false;
    //move robot back to original position
    arbotPlatform.reset(); //js only
    //js only display gene
    document.getElementById("raw").innerHTML = this.listCurrentGenome();
    //end js only display gene
  };


  this.initLoop=function()
  {
    console.log("initLoop, time from sequence start: ");
    sequenceDuration=(millis()-this.sequenceStartTimestamp);
    console.log(sequenceDuration);
    document.getElementById("timer").innerHTML = "time (s):" +sequenceDuration/1000;
    //real eeprom will be to slow for this
    if(sequenceDuration>EEPROM.read(EEPROM_GENERATION_TIMEOUT)){ //robot who doesn't find his way in 30' is a dead robot
      this.state=DEAD;
      return;
    }
    this.currentStep=GENOME_START;
    this.state=DONE_MOVE;
  }

  this.think=function(){
    this.arbotPlatform.fullStop();
    this.state=THINKING;
    this.interruptsDisabled=false;

    this.currentStep++;
    if(this.currentStep>=GENOME_LENGTH-1) {
      this.state=NEXT_LOOP;
      return;
    }

    gene=EEPROM.read(this.currentGenome*GENOME_LENGTH+this.currentStep);
    this.geneToCommand(gene);
  }

  this.move=function(command, time){
    this.state=MOVING;
    if(DONT_MOVE) command=G_NOP; //prevent moving... useful for debugging
    switch(command){
    case G_FORWARDS:
      this.arbotPlatform.forwards(100);
      break;
    case G_BACKWARDS:
      this.arbotPlatform.backwards(100);
      break;
    case G_LEFT:
      this.arbotPlatform.left(100);
      break;
    case G_RIGHT:
      this.arbotPlatform.right(100);
      break;
    case G_NOP:
      this.arbotPlatform.fullStop();
      break; //mkey...?
    }
    if((time & b("00011111"))==b("00011111")) {
      this.moveTimeout=0;
    }
    else {
      this.moveTimeout=time+1;
    }
    this.startedMovingTimestamp=millis();
  }

  this.checkFreedom=function(){
    moveTimer=millis()-this.startedMovingTimestamp; //freedom is moving straight without bumping into anything
    if(this.freedom){ //only sensor is good indicator of freedom
      this.state=FREE;
      //from this point on only some stats
      timer=millis()-this.sequenceStartTimestamp;
      this.generationTotalTime +=timer;
      this.generationFreeCount++;
      document.getElementById("avgtime").innerHTML = "average time:" +((this.generationTotalTime/this.generationFreeCount)/1000);
    }
    else if(!this.arbotPlatform.isMoving()){
      this.arbotPlatform.fullStop();
      this.state=STALLED;
    }
    else if ((moveTimer>=MOVE_TIMEOUT) || (this.moveTimeout!=0 && moveTimer>=this.moveTimeout*TIMEOUT_MULTIPLIER)){ //did we exceed move comand timer or are we spinning in circles?
      this.state=DONE_MOVE; //this will stop and move to next step
      console.log("move time exceeded");
    }
  }

  this.stallInterrupt=function()
  {
    this.interruptsDisabled=true;
    this.arbotPlatform.fullStop();
    gene=EEPROM.read(this.currentGenome*GENOME_LENGTH+GENOME_START-1); //this is the command for interrupt
    this.geneToCommand(gene);
    this.state=THINKING; //stall interrupt is just a different kind of thinking... will call move next
  }

  //Just to be consistant and use states
  this.setStateDoneMove=function()
  {
    this.state=DONE_MOVE;
  }

  this.finishSequence=function()
  {
    this.arbotPlatform.reset();//js only to move player back to beginning
    this.arbotPlatform.fullStop();
    //how long dit it take to solve the maze?
    sequenceDuration=(millis()-this.sequenceStartTimestamp); //js only millis, this is a bug in arduino code if we finish under a minute!
    console.log(sequenceDuration);
    award=-sequenceDuration; //js only, use duration directly, but store it negative so smaller is better
    if(!this.freedom) award=-9999999999;
    this.saveSuccessPoints(award);

    //Save best time
    bestTime=EEPROM.read(EEPROM_GENERATION_BEST_TIME);
    if(sequenceDuration<bestTime && this.freedom){
      EEPROM.write(EEPROM_GENERATION_BEST_TIME,sequenceDuration);
      document.getElementById("besttime").innerHTML = "best time (s):" +sequenceDuration*1000;
    }

    this.state=SEQUENCE_DONE;
  }

  this.switchGenome=function(){
    console.log("switch genome, new genome id:");
    //switching genome
    this.currentGenome++;
    console.log(this.currentGenome);
    this.saveCurrentGenomeId(this.currentGenome);

    if(this.currentGenome>=POPULATION_SIZE-1){
      this.state=GENERATION_DONE;
      return;
    }
    this.state=STARTING; //in simulator we do auto restart
  }

  //should be switchGeneration
  this.repopulate=function(){
    console.log("###REPOPULATING###");

    //sort by success
    genomes=[];
    //increase generation counter
    for(i=0; i<POPULATION_SIZE;i++)
    {
      offset=i*GENOME_LENGTH;
      genomes[i]={}; //js specific
      genomes[i].genome=i;
      genomes[i].health=EEPROM.read(offset+1);
      genomes[i].success=EEPROM.read(offset+2);
    }
    //qsort(genomes, POPULATION_SIZE, sizeof(struct genome), genome_cmp_desc);
    genomes=genomes.sort(function(a, b) {
        return ((a.success > b.success) ? -1 : ((a.success < b.success) ? 1 : 0));
      });

    console.log("sorted");

    //this will rewrite unsucessful genomes with new kids
    j=POPULATION_SIZE-1;
    populationHalf=POPULATION_SIZE/2
    for(i=0;i<populationHalf;i++)
    {
      mom=genomes[i].genome; //random(0,populationHalf)].genome;
      dad=genomes[random(0,populationHalf)].genome;
      kid=genomes[i+populationHalf].genome; //worst half of the population is repopulated
      console.log("mom:");
      console.log(mom);
      console.log("dad:");
      console.log(dad);
      console.log("kid");
      console.log(kid);

      m_generation=EEPROM.read(mom*GENOME_LENGTH);
      d_generation=EEPROM.read(dad*GENOME_LENGTH);
      EEPROM.write(kid*GENOME_LENGTH,(m_generation>d_generation?m_generation:d_generation)+1);
      //we randomly crossower dad's and mom's genes
      //trying a variation where the order stays the same, hence the same is applied to the whole genome
      //LOG
      //1. taking commands from one parent and times from the other doesn't seem very cool, the genes mutate to something not very sucessful
      //2. trying to split and take some genes from mother some from father to get new combinations
      //    -not very successful
      //i'm thinking my commands are vectors and crossing-over vectors makes no sense
      //3. let's try to take out cross-over and tune up mutations
      crossoverDice=random(GENOME_START-1,GENOME_LENGTH);
      for(k=GENOME_START-1; k<GENOME_LENGTH;k++){ //we start at interupt command
        console.log("gene ");
        console.log(k);
        gene;
        m_gene=EEPROM.read(mom*GENOME_LENGTH+k);
        d_gene=EEPROM.read(dad*GENOME_LENGTH+k);
        console.log("m_gene:");
        console.log(m_gene);
        console.log("d_gene:");
        console.log(d_gene);

        //let's only use mom
        gene=m_gene;
        /*2if(k<crossoverDice){
          gene=m_gene;
        } else {
          gene=d_gene;
        }*/

        /* 1
        switch(crossoverDice){

          case 0:
            console.log("using mom's command and dad's time");
            gene=(m_gene & G_MASK) | (d_gene & T_MASK);
            break;
          case 1:
            console.log("using dad's command and mom's time");
            gene=(d_gene & G_MASK) |( m_gene & T_MASK);
            break;
          ase 2:
            console.log("using dad's gene");
            gene=d_gene;
            break;
          default:
            console.log("using mom's gene");
            gene=m_gene;
            break;
        }*/
        console.log("kid's gene:");
        console.log(gene);
        //make mutants 20% chance as we are kindof low on time
        dice=random(0,100);
        if(dice<=20){
          console.log("MUTATING");
          console.log(gene);
          console.log("-->");
          dice=random(0,8);
          gene ^= 1 << dice; //this should randomly flip a single bit
        }
        console.log(gene);
        EEPROM.write(kid*GENOME_LENGTH+k,gene);
        console.log(kid*GENOME_LENGTH+k);
      }
      j--;
    }

    //reset health and success
    for(i=0; i<POPULATION_SIZE;i++)
    {
      offset=i*GENOME_LENGTH;
      EEPROM.write(offset+1,255); //reset health
      EEPROM.write(offset+2,0); //reset success
    }
    document.getElementById("gentimes").innerHTML = document.getElementById("gentimes").innerHTML +"<br/> generation:" + this.getCurrentGenerationId() +" best:"+EEPROM.read(EEPROM_GENERATION_BEST_TIME)+"goals: "+this.generationFreeCount;
    this.saveCurrentGenomeId(0);     //after repopulation, we start with first genome, we also set currentGenome to 0
    this.saveCurrentGenerationId(parseInt(this.getCurrentGenerationId())+1);  //increment generation counter

    //calculate time limit for next gen
    //a problem here is that freeCount is lost in case of reset, needs to go to eeprom
    if(this.generationFreeCount>=POPULATION_SIZE/2 && genomes[POPULATION_SIZE/2].success*-1.1<EEPROM.read(EEPROM_GENERATION_TIMEOUT) ){
      EEPROM.write(EEPROM_GENERATION_TIMEOUT,genomes[POPULATION_SIZE/2].success*-1.1); // last genome's time we keep is also the cutoff time (with some slack)
      document.getElementById("timeout").innerHTML=genomes[POPULATION_SIZE/2].success*-1.1/1000;
    }

    //init generation
    this.arbotPlatform.reset(); //js only
    this.state=STARTING; //in simulation we start from beginning
    this.generationTotalTime    = 0; //set average time back to 0
    this.generationFreeCount      = 0;
    document.getElementById("besttime").innerHTML="";

  }


  //command and time should only be set here to be consistent
  //we use them as parameter to move and as
  //memory to check what that command was
  this.geneToCommand=function(gene){
    if((gene & G_MASK)==G_REVERT){
      switch(this.lastCommand){
      case G_FORWARDS:
        this.lastCommand=G_BACKWARDS;
        break;
      case G_BACKWARDS:
        this.lastCommand=G_FORWARDS;
        break;
      case G_LEFT:
        this.lastCommand=G_RIGHT;
        break;
      case G_RIGHT:
        this.lastCommand=G_LEFT;
        break;
      default:
        this.lastCommand=G_NOP;
        break;
      }
    }
    else{
      this.lastCommand=gene & G_MASK;
    }
    this.lastTime=gene & T_MASK;
  }

  

  this.setup=function(){
    if(SEED_GENOME) {
      this.seedGenome();
    }
    //TODO
    this.listGenome();
    if(SIM_REPOPULATE){

      for(i=0;i<10;i++){
        //TODO
        this.repopulate();
        this.listGenome();
      }
    }
    //TODO
    //attachInterrupt(FREEDOM_INTERRUPT,freedomInterrupt,LOW);
  }

  this.listGenome=function()
  {
    console.log("current Genome Id:");
    console.log(this.getCurrentGenomeId());
    console.log("current generation Id:");
    console.log(this.getCurrentGenerationId());
    //
    for(i=0;i<POPULATION_SIZE;i++)
    {
      console.log("genome:");
      console.log(i);
      offset=i*GENOME_LENGTH;
      //b0, generation;
      console.log(EEPROM.read(offset+0));
      console.log(",");
      //b1, health
      console.log(EEPROM.read(offset+1));
      console.log(",");
      //b2, success;
      console.log(EEPROM.read(offset+2));
      console.log(":");
      //random algorythim
      for(j=GENOME_START-1 ;j<GENOME_LENGTH;j++) //we start at interupt command
      {
        gene=EEPROM.read(i*GENOME_LENGTH+j);
        console.log(gene);
        console.log("-");
        command=gene & G_MASK;
        time=gene & T_MASK;
        switch(command){
        case G_FORWARDS:
          console.log("fwd");
          break;
        case G_BACKWARDS:
          console.log("bwd");
          break;
        case G_LEFT:
          console.log("lft");
          break;
        case G_RIGHT:
          console.log("rgh");
          break;
        case G_NOP:
          console.log("nop");
          break;
        case G_REVERT:
          console.log("revert");
          break;
          //case 5: command=PAUSE;break; //maybe in the future
        }
        if((time & b("00011111"))==b("00011111")) {
          console.log("NOLIM");
        }
        else {
          console.log(time);
        }
        console.log(":");
      }
      console.log("");
    }
  }

   // initialises genome in eprom
  //genome structure, bytes:
  //b0                 = generation
  //b1                 = health
  //b2                 = success
  //b3                 = command for bump interriupt
  //b4-bGENOME_LENGTH  = algorythm
  this.seedGenome=function()
  {
    for(i=0;i<POPULATION_SIZE;i++)
    {
      console.log("seeding genome:");
      console.log(i);
      offset=i*GENOME_LENGTH;
      //b0, generation
      EEPROM.write(offset+0,0);
      console.log(0);
      //b1, health
      EEPROM.write(offset+1,255);
      console.log(255);
      //b2, success
      EEPROM.write(offset+2,0);
      console.log(0);
      console.log(":");
      //b3 stall interrupt - we generate stall interrupt randomly in this process
      //random algorythim
      for(j=3;j<GENOME_LENGTH;j++)
      {
        command =0;
        speed =0;
        //throw the dice for command
        dice=1;
        while(dice==0 || dice==1 || dice==7){ //commands 0b001 and 0b111 is not implemented at the moment and let's ignore NOP
          dice = random(0,8);
        }
        // console.log("int command:");
        //console.log(dice);
        //console.log(dice,BIN);
        command=dice<<5; //simply shift dice by 5 to get to the command
        //console.log(command,BIN);
        //moving till we hit sth or time based move?
        dice= random(0,2);
        if(dice==0 && !(command==G_LEFT ||command==G_RIGHT)){ //don't allow unlimited circling
          speed=TIL_HIT;
          console.log("NOLIM");
        }
        else {
          speed=random(0,31);
          console.log(speed);
        }
        /*
        if(i==0 && j==3){ //let's seed REVERSE interrupt handler for this one genome, just to make sure we have one
          console.log("OVERRIDE");
          command=G_REVERT;
          speed=5;
        }*/
        //console.log(command | speed,BIN);
        console.log(":");
        EEPROM.write(offset+j,command | speed);
      }
      console.log("");
    }
    //this is first run init block, important as it sets EEPROM
    this.saveCurrentGenomeId(0); //when wee seed genome, we also set currentGenome to 0
    this.saveCurrentGenerationId(0);
    EEPROM.write(EEPROM_GENERATION_BEST_TIME,999999999999);
    EEPROM.write(EEPROM_GENERATION_TIMEOUT,FIRST_GENERATION_TIMOUT );
  }

  

  this.listCurrentGenome=function(){
    output="";
    for(j=GENOME_START-1 ;j<GENOME_LENGTH;j++) //we start at interupt command
    {
        gene=EEPROM.read(this.currentGenome*GENOME_LENGTH+j);
        output+=gene;
        output+="-";
        command=gene & G_MASK;
        time=gene & T_MASK; 
        switch(command){
        case G_FORWARDS: 
          output+="fwd"; 
          break;
        case G_BACKWARDS: 
          output+="bwd"; 
          break;
        case G_LEFT: 
          output+="lft";
          break;
        case G_RIGHT: 
          output+="rgh";
          break;
        case G_NOP: 
          output+="nop";
          break;
        case G_REVERT:
          output+="revert";
          break;
          //case 5: command=PAUSE;break; //maybe in the future
        }
        if((time & b("00011111"))==b("00011111")) { 
          output+="NOLIM";
        } 
        else {
          output+=time;
        }
        output+=":";
      }
      return output;
  }
  

  this.blink=function(count){
    //NOT implemented
  }

  //id is simply the sequence number of the genome
  this.saveCurrentGenerationId=function(currentGenerationId)
  {
    EEPROM.write(EEPROM_CURRENT_GENERATIONID_ADDR,currentGenerationId);
    document.getElementById("generation").innerHTML = "current generation:" +currentGenerationId;
  }

  this.getCurrentGenerationId=function()
  {
    currentGenerationId = EEPROM.read(EEPROM_CURRENT_GENERATIONID_ADDR);
    document.getElementById("generation").innerHTML = "current generation:" +currentGenerationId;
    return currentGenerationId;

  }

  //id is simply the sequence number of the genome
  this.saveCurrentGenomeId=function(currentGenomeId)
  {
    EEPROM.write(EEPROM_CURRENT_GENOMEID_ADDR,currentGenomeId);
    document.getElementById("genome").innerHTML = "current genome:" +currentGenomeId;
  }

  this.getCurrentGenomeId=function()
  {
    currentGenomeId=EEPROM.read(EEPROM_CURRENT_GENOMEID_ADDR);
    document.getElementById("genome").innerHTML = "current genome:" +currentGenomeId;
    return currentGenomeId;
  }

  this.saveSuccessPoints=function(award){
    EEPROM.write(this.currentGenome*GENOME_LENGTH+2,award);
  }

}
