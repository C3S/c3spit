/* 
  A Minimal Capture Program
  This program opens an audio interface for capture, configures it for
  stereo, 16 bit, 44.1kHz, interleaved conventional read/write
  access. Then its reads a chunk of random data from it, and exits. It
  isn't meant to be a real program.
  From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html
  Fixes rate and buffer problems
  sudo apt-get install libasound2-dev
  gcc -o c3spit c3spit.c -lasound & ./c3spit hw:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
	      
main (int argc, char *argv[])
{
  int i;
  int err;
#define NO_OF_BUFFER 10
  char *buffer[NO_OF_BUFFER];
  int buffer_frames = 163840;
  unsigned int rate = 44100;
  int channels = 2;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
             argv[1],
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "audio interface opened\n");
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params allocated\n");
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params initialized\n");
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params access set\n");
	
  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params format set\n");
	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }
	
  fprintf(stdout, "hw_params rate set\n");

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, channels)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params channels set\n");
	
  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params set\n");
	
  snd_pcm_hw_params_free (hw_params);

  fprintf(stdout, "hw_params freed\n");
	
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "audio interface prepared\n");

  int buffer_size = buffer_frames * snd_pcm_format_width(format) / 8 * 2;
  for (i = 0; i < NO_OF_BUFFER; ++i) {
    buffer[i] = malloc(buffer_size);
    if ((err = snd_pcm_readi (capture_handle, buffer[i], buffer_frames)) != buffer_frames) {
      fprintf (stderr, "read from audio interface failed (%s)\n",
               snd_strerror (err));
      exit (1);
    }
    fprintf(stdout, "recording in progress: %d0%% done\n", i+1);
  }

  fprintf(stdout, "buffer freed\n");
	
  snd_pcm_close (capture_handle);
  fprintf(stdout, "audio interface closed\n");

  // WAV file output
  
  FILE *fp = NULL;
  fp = fopen("samples.wav", "w");
  if (!fp)
  {
      fprintf(stdout, "can't open file for writing\n");      
      exit (1);
  }
  
  struct {
    char chunkID[4];
    uint32_t chunkSize; 
    char riffType[4];      
    char fmtSubChunkID[4];
    uint32_t fmtSubChunkSize;       
    uint16_t wFormatTag;
    uint16_t wChannels;
    uint32_t dwSamplesPerSec;
    uint32_t dwAvgBytesPerSec;
    uint16_t wBlockAlign;
    uint16_t wBitsPerSample;
    char dataSubChunkID[4];
    uint32_t dataSubChunkSize;
  } wavHeader;
  
  // fill header
  memcpy(wavHeader.chunkID, "RIFF", 4);
  wavHeader.chunkSize = 36 /*header*/ + buffer_size * NO_OF_BUFFER; 
  memcpy(wavHeader.riffType, "WAVE", 4);
  memcpy(wavHeader.fmtSubChunkID, "fmt ", 4);
  wavHeader.fmtSubChunkSize = 16;
  wavHeader.wFormatTag = 1; // generic PCM
  wavHeader.wChannels = channels;
  wavHeader.dwSamplesPerSec = rate;
  wavHeader.wBlockAlign = 2 * channels; // ASSERT(format == SND_PCM_FORMAT_S16_LE) !
  wavHeader.dwAvgBytesPerSec = wavHeader.dwSamplesPerSec * wavHeader.wBlockAlign; // 16 bit -- ASSERT(format == SND_PCM_FORMAT_S16_LE) !
  wavHeader.wBitsPerSample = 16; // ASSERT(format == SND_PCM_FORMAT_S16_LE) !
  memcpy(wavHeader.dataSubChunkID, "data", 4);
  wavHeader.dataSubChunkSize = buffer_size * NO_OF_BUFFER;
  fwrite(&wavHeader, 1, sizeof(wavHeader), fp);

  // fill audio data
  for (i = 0; i < NO_OF_BUFFER; ++i) {
    if (fwrite(buffer[i], 1, buffer_size, fp) < buffer_size)
    {
      fprintf(stdout, "error writing to file (disk full?)\n");  
      fclose(fp);          
      exit (1);        
    }
    free(buffer[i]);    
  }  
  
  fclose(fp);
  
  exit (0);
}
