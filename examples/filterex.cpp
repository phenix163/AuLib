/////////////////////////////////////////////////////////////////////
// Copyright (C) 2016-7 V Lazzarini
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3.0 of the License, or (at your option) any later version.
//
/////////////////////////////////////////////////////////////////////
#include <Balance.h>
#include <BlOsc.h>
#include <ResonR.h>
#include <SoundOut.h>
#include <iostream>

using namespace AuLib;
using namespace std;

int main() {

  TableSet saw(SAW);
  BlOsc sig(0.5, 440., saw);
  ResonR fil(1000, 1.);
  Balance bal;
  SoundOut output("dac");

  cout << Info::version();
  if (sig.error() == AULIB_NOERROR) {
    if (fil.error() == AULIB_NOERROR) {
      if (bal.error() == AULIB_NOERROR) {
        if (output.error() == AULIB_NOERROR) {
          for (int i = 0; i < def_sr * 10; i += def_vframes) {
            sig.process(0.5, 440.);
            fil.process(sig, 1000. + 400. * i / def_sr);
            bal.process(fil, sig);
            output.write(bal);
          }
        } else
          cout << output.error_message() << "\n";
      } else
        cout << bal.error_message() << "\n";
    } else
      cout << fil.error_message() << "\n";
  } else
    cout << sig.error_message() << "\n";

  return 0;
}
