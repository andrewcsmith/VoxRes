VoxRes : MultiOutUGen {
  *kr { 
    arg in, lpcCount = 16;
    ^this.multiNew('control', in, lpcCount)
  }

  init {
    arg ... theInputs;
    inputs = theInputs;
    ^this.initOutputs(4, rate);
  }
}
