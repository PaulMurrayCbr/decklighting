import { Injectable } from '@angular/core';
import { Observable, throwError } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';
import { HttpClient } from '@angular/common/http';

export class Master {
	status: String = "";
	brightness: number;
}

export class Room {
	status: String = "";
	density: number = 1;
	c1: String = "";
	c2: String = "";
	interp: String = "";
	effect: String = "";
}

export class Data {
	master: Master = new Master();
	room: Array<Room> = [ null, new Room(), new Room(), new Room(), new Room(), new Room() ];
}


@Injectable({
  providedIn: 'root'
})
export class StripService {

  data: Data = new Data();

  constructor(private http: HttpClient) { 
  	console.log("constructing, calling get");
  	http.get("http://localhost:8080/decklighting-stub/on?room=all").subscribe((data) => { this.data.master = data as Master; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=1").subscribe((data) => { this.data.room[1] = data as Room;
  	http.get("http://localhost:8080/decklighting-stub/on?room=2").subscribe((data) => { this.data.room[2] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=3").subscribe((data) => { this.data.room[3] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=4").subscribe((data) => { this.data.room[4] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=5").subscribe((data) => { this.data.room[5] = data as Room; 
  	});
  	});
  	});
  	});
  	});
  	});
  
  } 
  
  updateMasterBrightness(n:number) {
  	this.http.get("http://localhost:8080/decklighting-stub/on?room=all&brightness="+n).subscribe((data) => { this.data.master = data as Master; });
  }
  
  refreshMaster() {
  	this.http.get("http://localhost:8080/decklighting-stub/?room=all").subscribe((data) => { this.data.master = data as Master; });
  }
  
  refreshRoom(n: number) {
  	this.http.get("http://localhost:8080/decklighting-stub/?room="+n).subscribe((data) => { this.data.room[n] = data as Room; });
  }
  
  on(n:number) {
  	if(n==0)
	  	this.http.get("http://localhost:8080/decklighting-stub/on?room=all").subscribe((data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get("http://localhost:8080/decklighting-stub/on?room="+n).subscribe((data) => { this.data.room[n] = data as Room; });
  }
  
  off(n:number) {
  	if(n==0)
	  	this.http.get("http://localhost:8080/decklighting-stub/off?room=all").subscribe((data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get("http://localhost:8080/decklighting-stub/off?room="+n).subscribe((data) => { this.data.room[n] = data as Room; });
  }
  
  out(n:number) {
  	if(n==0)
	  	this.http.get("http://localhost:8080/decklighting-stub/out?room=all").subscribe((data) => { this.data.master = data as Master; });
  	else 
	  	this.http.get("http://localhost:8080/decklighting-stub/out?room="+n).subscribe((data) => { this.data.room[n] = data as Room; });
  }
  
  effect(n:number, e: String) {
	  	this.http.get("http://localhost:8080/decklighting-stub/effect?room="+n+"&effect="+e).subscribe((data) => { this.data.room[n] = data as Room; });
  
  }
  
}
