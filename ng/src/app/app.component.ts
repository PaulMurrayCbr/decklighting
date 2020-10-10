import { Component } from '@angular/core';
import { StripService } from './strip.service';
import { Data } from './strip.service';

class RoomUi {
}

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
	title = 'ng';

	c1: String ="#39efb8";
	c2: String ="#00ff00";

	data: Data;
	tab: number = 0;
	roomTab: String = 'state';
	room: Array<RoomUi> = [
		null,
		new RoomUi(),
		new RoomUi(),
		new RoomUi(),
		new RoomUi(),
		new RoomUi()
	];
	
	roomasstring() {
	  return JSON.stringify(this.data.room[this.tab]);
	}
	 
	constructor(private stripService: StripService) {
		this.data = stripService.data;
	}
	
	br(n: number) {
		this.stripService.updateMasterBrightness(n);
	}
	
	isBr(n: number) {
		return this.data.master.brightness == n;
	}
	
	allOff() {
		if(this.data.master.status == "off")
			this.stripService.on(0);
		else 
			this.stripService.off(0);
	}
	
	refresh() {
		if(this.tab==0)
			this.stripService.refreshMaster();
		else
			this.stripService.refreshRoom(this.tab);
	}
	effect(e: String) {
		this.data.room[this.tab].effect = e;
	
		this.stripService.effect(this.tab, e);
	}
	
	isOn(n: number) {
		return this.data.room[n].status == 'on';
	}
	isOff(n: number) {
		return this.data.room[n].status == 'off';
	}
	isOut(n: number) {
		return this.data.room[n].status == 'out';
	}
	on(n: number) {
		return this.stripService.on(n);
	}
	off(n: number) {
		return this.stripService.off(n);
	}
	out(n: number) {
		if(this.tab==n) this.tab = 0;
		return this.stripService.out(n);
	}
	c1change(e: Event) {
		return this.stripService.color1(this.tab, this.data.room[this.tab].c1);
	}
	c2change(e: Event) {
		return this.stripService.color2(this.tab, this.data.room[this.tab].c2);
	}
	white() {
		return this.stripService.bothcolors(this.tab, "#FFB090");
	}
	
	interpSelectChange() {
	  return this.stripService.interp(this.tab, this.data.room[this.tab].interp);
	}
	effectSelectChange() {
	  this.stripService.effect(this.tab, this.data.room[this.tab].effect);
	}
	isDensity(a: number) {
	  return this.data.room[this.tab].density == a;
	}
	density(a: number) {
	  return this.stripService.density(this.tab, a);
	}

    speedSeconds(n :number) {
      return Math.exp(n/1000 * (Math.log(60)-Math.log(.1)) + Math.log(.1));
    }	
    
    bballChange() {
		return this.stripService.updateBBall(this.tab, this.data.room[this.tab].effectData.bouncyball);   	
    }
    flickerChange() {
		return this.stripService.updateFlicker(this.tab, this.data.room[this.tab].effectData.flicker);   	
    }
}
