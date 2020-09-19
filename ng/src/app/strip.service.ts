import { Injectable } from '@angular/core';
import { Observable, throwError } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';
import { HttpClient } from '@angular/common/http';

export class Master {
	status: String;
	brightness: String;
}

export class Room {
	status: String;
	density: int;
	c1: String;
	c2: String;
	interp: String;
	effect: String;
}

export class Data {
	master: Master = new Master();
	room: Array<Room> = [ new Room(), new Room(), new Room(), new Room(), new Room() ];
}


@Injectable({
  providedIn: 'root'
})
export class StripService {

  data: Data = new Data();

  constructor(private http: HttpClient) { 
  	console.log("constructing, calling get");
  	http.get("http://localhost:8080/decklighting-stub/on?room=all").subscribe((data) => { this.data.master = data as Master; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=0").subscribe((data) => { this.data.room[0] = data as Room;
  	http.get("http://localhost:8080/decklighting-stub/on?room=1").subscribe((data) => { this.data.room[1] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=2").subscribe((data) => { this.data.room[2] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=3").subscribe((data) => { this.data.room[3] = data as Room; 
  	http.get("http://localhost:8080/decklighting-stub/on?room=4").subscribe((data) => { this.data.room[4] = data as Room; 
  	});
  	});
  	});
  	});
  	});
  	});
  
  } 
  
  
  
}
