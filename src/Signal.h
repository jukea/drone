typedef float Signal_T;
typedef Signal_T* Signal_T_ptr;

#define clipSignal(sig) ((sig < -1.0f) ? -1.0f : ((sig > 1.0f) ? 1.0f : sig))
#define signalToUChar(sig) ((unsigned char)((sig+1.0f)*128.0f))
#define signalToFrequency(sig) (sig*10000.0f)

