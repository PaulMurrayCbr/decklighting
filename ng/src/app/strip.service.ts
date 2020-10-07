import { Injectable } from '@angular/core';
import { Observable, throwError } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';
import { HttpClient } from '@angular/common/http';

export class Master {
	status: String = "";
	brightness: number;
}

export class EffectRainbow {
}

export class EffectTheatre {
}

export class EffectPlasma {
}

export class EffectBouncyBall {
	width: number; // width is in pixels
	speed: number; // speed is in pixels per second
}

export class EffectFlicker {
	c1Rate: number; // the probability that over a second, the primary colour will be set
	c2Rate: number; // the probability that over a second, the primary colour will be set
}

export class Room {
	status: String = "";
	density: number = 1;
	c1: String = "";
	c2: String = "";
	interp: String = "";
	effect: String = "";
	effectData: {
		plasma: EffectPlasma;
	    bouncyBall: EffectBouncyBall;
	    flicker: EffectFlicker;
	    rainbow: EffectRainbow;
	    theatre: EffectTheatre;
    }
}

export class Data {
	master: Master = new Master();
	room: Array<Room> = [ null, new Room(), new Room(), new Room(), new Room(), new Room() ];
}


@Injectable({
  providedIn: 'root'
})
export class StripService {

  // base = "http://192.168.0.149:8080/decklighting-stub";
  base = "http://192.168.0.14";

  data: Data = new Data();

  constructor(private http: HttpClient) { 
  	console.log("constructing, calling get");
  	http.get<Master>(this.base+"/status?room=all").subscribe((data) => { this.data.master = data as Master; 
  	http.get<Room>(this.base+"/status?room=1").subscribe((data) => { this.data.room[1] = data as Room;
  	http.get<Room>(this.base+"/status?room=2").subscribe((data) => { this.data.room[2] = data as Room; 
  	http.get<Room>(this.base+"/status?room=3").subscribe((data) => { this.data.room[3] = data as Room; 
  	http.get<Room>(this.base+"/status?room=4").subscribe((data) => { this.data.room[4] = data as Room; 
  	http.get<Room>(this.base+"/status?room=5").subscribe((data) => { this.data.room[5] = data as Room; 
  	});
  	});
  	});
  	});
  	});
  	});
  
  } 
  
  updateMasterBrightness(n:number) {
  	this.http.get<Master>(this.base+"/on?room=all&brightness="+n).subscribe(
  		(data) => { this.data.master = data as Master; });
  }
  
  refreshMaster() {
  	this.http.get<Master>(this.base+"/status?room=all").subscribe(
  		(data) => { this.data.master = data as Master; });
  }
  
  refreshRoom(n: number) {
  	this.http.get<Room>(this.base+"/status?room="+n).subscribe(
  		(data) => { this.data.room[n] = data as Room; });
  }
  
  on(n:number) {
  	if(n==0)
	  	this.http.get<Master>(this.base+"/on?room=all").subscribe(
	  	(data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get<Room>(this.base+"/on?room="+n).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }
  
  off(n:number) {
  	if(n==0)
	  	this.http.get<Master>(this.base+"/off?room=all").subscribe(
	  		(data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get<Room>(this.base+"/off?room="+n).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }
  
  out(n:number) {
  	if(n==0)
	  	this.http.get<Master>(this.base+"/out?room=all").subscribe(
	  		(data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get<Room>(this.base+"/out?room="+n).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }
  
  density(n:number, a: number) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&density="+a).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  
  }
  
  effect(n:number, e: String) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&effect="+e).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }
  
  color1(n:number, c: String) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&c1=%23"+c.substr(1)).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }
  
  color2(n:number, c: String) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&c2=%23"+c.substr(1)).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }

  bothcolors(n:number, c: String) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&c1=%23"+c.substr(1)+"&c2=%23"+c.substr(1)).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  }

  interp(n:number, e: String) {
	  	this.http.get<Room>(this.base+"/on?room="+n+"&interp="+e).subscribe(
	  		(data) => { this.data.room[n] = data as Room; });
  
  }
  
}
