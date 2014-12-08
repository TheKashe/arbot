function ArbotPhaserPlatform(player){

    var SPEED=1500;

    this.player=player;

    this.forwards=function(speed){
	  this.player.body.velocity.x = Math.cos(toRadians(this.player.body.rotation))*SPEED;
      this.player.body.velocity.y = Math.sin(toRadians(this.player.body.rotation))*SPEED;
    }
    this.backwards=function(speed){
      this.player.body.velocity.x = Math.cos(toRadians(this.player.body.rotation))*-SPEED;
      this.player.body.velocity.y = Math.sin(toRadians(this.player.body.rotation))*-SPEED;    
    }

    this.left=function(speed){
       this.player.body.angularVelocity=1500;
       this.player.body.velocity.x = Math.cos(toRadians(this.player.body.rotation))*1;
      this.player.body.velocity.y = Math.sin(toRadians(this.player.body.rotation))*1;
    }
    
    this.right=function(speed){
      this.player.body.angularVelocity=-1500;
      this.player.body.velocity.x = Math.cos(toRadians(this.player.body.rotation))*1;
      this.player.body.velocity.y = Math.sin(toRadians(this.player.body.rotation))*1;
    }
    
    this.fullStop=function(){
      this.player.body.angularVelocity=0;
      this.player.body.velocity.x = 0;
      this.player.body.velocity.y = 0;
    }

    this.isMoving=function()
    {
      if(this.player.body.velocity.x==0 && this.player.body.velocity.y==0) return false;
      return true;

    }
};


function toRadians (angle) {
  return angle * (Math.PI / 180);
}