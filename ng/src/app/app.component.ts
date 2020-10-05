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
		return this.stripService.out(n);
	}
	c1change(e: Event) {
		return this.stripService.color1(this.tab, this.data.room[this.tab].c1);
	}
	c2change(e: Event) {
		return this.stripService.color2(this.tab, this.data.room[this.tab].c2);
	}
	interpSelectChange(e: Event) {
	  return this.stripService.interp(this.tab, this.data.room[this.tab].interp);
	}
	isDensity(a: number) {
	  return this.data.room[this.tab].density == a;
	}
	density(a: number) {
	  return this.stripService.density(this.tab, a);
	}
	
}