//////////////////////////////////////////////////////////////////////
// SoundOut class implementation
// dependencies: libsndfile, portaudio
// Copyright (C) 2016-7 V Lazzarini
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3.0 of the License, or (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////
#include "SoundOut.h"
#include <sndfile.h>
#include <portaudio.h>
#include <iostream>

SoundOut::SoundOut(const char *dest, uint32_t nchnls,
		   uint32_t vsize, uint32_t bsize, double sr) :
  m_dest(dest), m_bsize(bsize), m_buffer(NULL),
  m_handle(NULL), m_mode(0), m_cnt(0),
  m_framecnt(0), AudioBase(nchnls,vsize,sr)
{
  if(strcmp("dac",m_dest) == 0){
    // RT audio
    PaError err;
    m_buffer = (void *) new float[m_bsize*m_nchnls];
    if(m_buffer == NULL) {
      m_bsize = 0;
      m_vsize = 0;
      return;
    }
    err = Pa_Initialize();
    if(err == paNoError){
      PaStreamParameters outparam;
      PaStream *stream;
      memset(&outparam, 0,
	     sizeof(PaStreamParameters));
      outparam.device = (PaDeviceIndex)
	Pa_GetDefaultOutputDevice();
      outparam.channelCount = m_nchnls;
      outparam.sampleFormat = paFloat32;
      outparam.suggestedLatency = (PaTime)
	(m_bsize/m_sr);
      err = Pa_OpenStream(&stream,NULL,&outparam,
			  m_sr,m_bsize,paNoFlag, 
			  NULL, NULL);
      if(err == paNoError){
	err = Pa_StartStream(stream);
	if(err == paNoError){
	  m_handle = (void *) stream;
	  m_mode = SOUNDOUT_RT;
	} else {
	  m_error = AULIB_RTSTREAM_ERROR;
	  m_vsize  = 0;
        }
      } else {
	m_error = AULIB_RTOPEN_ERROR;
	m_vsize  = 0;
      }
    } else {
      m_error = AULIB_RTINIT_ERROR;
      m_vsize  = 0;
    }
  }
  else if(strcmp("stdout",m_dest) == 0){
    // stdout
    m_mode = SOUNDOUT_STDOUT;
  }
  else {
    // sndfile
    SF_INFO info;
    info.samplerate = (int) m_sr;
    info.channels = m_nchnls;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    m_buffer = (void *) new double[m_bsize*m_nchnls];
    SNDFILE *sf = sf_open(m_dest,SFM_WRITE, &info);
    if(sf != NULL) {
      m_handle = (void *) sf;
      m_mode = SOUNDOUT_SNDFILE;
    } else {
      m_error = AULIB_FOPEN_ERROR;
      m_vsize = 0;
    }
  }
}

SoundOut::~SoundOut(){
  if(m_mode == SOUNDOUT_RT &&
     m_handle != NULL) {
    Pa_StopStream((PaStream*) m_handle);
    Pa_CloseStream((PaStream*) m_handle);
    Pa_Terminate();
    float *buffer = (float *) m_buffer;
    delete[] buffer;
  }
  else if(m_mode == SOUNDOUT_SNDFILE &&
	  m_handle != NULL){
    sf_close((SNDFILE *) m_handle);
    double *buffer = (double *) m_buffer;
    delete[] buffer;
  }
}

uint32_t SoundOut::write(const double *sig){
  uint32_t samples = m_vsize*m_nchnls;
  if(m_mode == SOUNDOUT_RT &&
     m_handle != NULL) {
    PaError err;
    uint32_t bsamples = m_bsize*m_nchnls;
    float *buffer = (float *) m_buffer;
    for(int i = 0; i < samples; i++) {
      buffer[m_cnt++] = sig[i];
      if(m_cnt == bsamples){
	err = Pa_WriteStream((PaStream*) m_handle,
			     buffer,m_bsize);
	if(err == paNoError){
	  m_framecnt += m_cnt/m_nchnls;
	}
	m_cnt = 0;
      }
    }
  }
  else if(m_mode == SOUNDOUT_STDOUT){
    uint32_t sample_cnt = 0;
    for(int i = 0; i < samples; i++){
      std::cout << sig[i] << "\n";
      sample_cnt++;
    }
    m_framecnt += sample_cnt/m_nchnls;
  }
  else if(m_mode == SOUNDOUT_SNDFILE &&
	  m_handle != NULL) {
    uint32_t bsamples = m_bsize*m_nchnls;
    double *buffer = (double *) m_buffer;
    for(int i = 0; i < samples; i++) {
      buffer[m_cnt++] = sig[i];
      if(m_cnt == bsamples) {
	m_framecnt +=
	  sf_writef_double((SNDFILE*) m_handle,
			   buffer, m_bsize);
	m_cnt = 0;
      }
    }
  }
  else return 0;
  return m_framecnt;
}
