/* Sustain Pedal Simulator version: 1.0
    By Benedito Portela
    benep2@gmail.com
    December 2025
    How to compile:
    gcc pedalsim.c -o pedalsim -lasound
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

uint8_t lb=1, pedal=0; // Pedal state
uint8_t Keys[128]; // Keys

int main()
{
      for (uint8_t i=1;i<128;i++)	{ // All Keys in normal
      Keys[i]=1;
      }; 
//initializing the ALSA Ports
  snd_seq_t *seq_handle;
  snd_seq_event_t *ev;
  int i,k;
  int portid;              /* input port */
  int oportid;         /* output ports */
  int npfd;
  struct pollfd *pfd;

  if (snd_seq_open(&seq_handle, "hw", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    fprintf(stderr, "Error opening ALSA sequencer.\n");
    exit(1);
  }

  snd_seq_set_client_name(seq_handle, "PedalSim");

  /* Opennig the ports */
  if ((portid = snd_seq_create_simple_port
       (seq_handle, "Input",
        SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
    fprintf(stderr, "fatal error: could not open input port.\n");
    exit(1);
  }

    if ((oportid= snd_seq_create_simple_port
         (seq_handle, "PedalSim",
          SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
          SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
      fprintf(stderr, "fatal error: could not open output port.\n");
      exit(1);
    }


  npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
  pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
  snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);

  while (1)  /* main loop */
    if (poll(pfd, npfd, 1000000) > 0){
      do {
        snd_seq_event_input(seq_handle, &ev);
//Releasing all Keys
  if ((ev->type==SND_SEQ_EVENT_CONTROLLER && ev->data.control.param==64) && ev->data.control.value==0 )
                  {
                   for (uint8_t i=20; i<128; i++) {
			if ( Keys[i]==0) {
                    	 ev->type=SND_SEQ_EVENT_NOTEOFF;
                	     ev->data.note.note=i;
               	         snd_seq_ev_set_source( ev, oportid );
              		     snd_seq_ev_set_subs( ev );
             		     snd_seq_ev_set_direct( ev );
            		     snd_seq_event_output_direct( seq_handle, ev );
                	     snd_seq_free_event(ev);
                             Keys[i]=1; //Turn on Key
                              }
                                             }
                     pedal=0;
                     lb=0;
                   }
  // Pedal Event
         if ((ev->type==SND_SEQ_EVENT_CONTROLLER && ev->data.control.param==64) && ev->data.control.value==127)
             {
               lb=0;
               pedal=1; //Enables note capture.
              }
//Check and secure the notes if pedal ON
        if (ev->type==SND_SEQ_EVENT_NOTEOFF && pedal==1)
            {
            Keys[ev->data.note.note]=0;
            lb=0;
            }
//Send the flow if 'lb' is at 1
        if (lb==1)
                {
                   if (ev->type==SND_SEQ_EVENT_NOTEON && pedal==1)
                    {
                       Keys[ev->data.note.note]=1;
		               ev->type=SND_SEQ_EVENT_NOTEOFF;
		               snd_seq_ev_set_source( ev, oportid );
                       snd_seq_ev_set_subs( ev );
                       snd_seq_ev_set_direct( ev );
                       snd_seq_event_output_direct( seq_handle, ev );
                       ev->type=SND_SEQ_EVENT_NOTEON;
		             }
//Normal keys and others events 
                  snd_seq_ev_set_source( ev, oportid );
                  snd_seq_ev_set_subs( ev );
                  snd_seq_ev_set_direct( ev );
                  snd_seq_event_output_direct( seq_handle, ev );
                  }
          snd_seq_free_event(ev);
          lb=1;

      } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
    }
  return 0;
}

