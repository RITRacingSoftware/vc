#ifndef VC_HANDLE_H
#define VC_HANDLE_H
extern "C"{
void init(void);

void deinit(void);

void begin_logging(char* filename);

void run_ms(int ms);

void set(char* key, float val);

float get(char* key);

void inject_mc_state_msg(int state, bool enabled);

void inject_pbx_status_msg(bool pumps_on);
}
#endif // VC_HANDLE_H