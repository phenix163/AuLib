////////////////////////////////////////////////////////////////////
// Phasor class: phase signal generator
// Copyright (C) 2016-7 V Lazzarini
//
// This software is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
// version 3.0 of the License, or (at your option) any later version.
//
/////////////////////////////////////////////////////////////////////
#ifndef _PHASOR_H
#define _PHASOR_H

#include "AudioBase.h"

/** Phase signal (ramp) generator
 */
class Phasor : public AudioBase {

 protected:
  double m_phs;  
  double m_freq;
  double m_incr;
  
  /** phase modulo
   */
  void mod(){
    while(m_phs >= 1.0) m_phs -= 1.;
    while(m_phs < 0.0) m_phs += 1.;
  }

 public:
  /** Phasor constructor \n\n
      freq - frequency in Hz \n
      phase - init phase (0-1) \n 
      sr - sampling rate \n
      vsize - vector size \n
  */
  Phasor(double freq,
	double phase = .0,
	double sr = def_sr,
	uint32_t vsize = def_vsize);

  /** Process one vector of audio
   */
  virtual void process();
  
  /** Process one vector of audio with
      frequency freq
  */
  virtual void process(double freq){
    m_freq = freq;
    m_incr = m_freq/m_sr;
    process();
  }

};
#endif
