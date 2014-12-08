var EEPROM_SIZE=1024;

var EEPROM= new function(){

	this.read=function(address){
		return parseFloat(localStorage.getItem("gene"+address));
	}

	this.write=function(address,val){
		localStorage.setItem("gene"+address,val);
	}
}


//arduino's random
function random(min,max)
{
    return Math.floor(Math.random()*(max-min+1)+min);
}

function millis()
{
	return Date.now();
}