import { Component } from '@angular/core';
import { StripService } from './strip.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
	title = 'ng';

	data={};

	constructor(private stripService: StripService) {
		this.data = stripService.data;
	}
	
}
