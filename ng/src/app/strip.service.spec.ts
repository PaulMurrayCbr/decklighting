import { TestBed } from '@angular/core/testing';

import { StripService } from './strip.service';

describe('StripService', () => {
  let service: StripService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(StripService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
