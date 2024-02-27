void cli_load_xxxx(void)
    {
    
  ESC_STATE *s = &esc_state;
  int i = 0;
  
  s->store[i++] = 0x03400351;
  s->store[i++] = 0x03620363;
  s->store[i++] = 0x03640365;
  s->store[i++] = 0x03660367;
  s->store[i++] = 0x03680369;
  s->store[i++] = 0x03611056;
  s->store[i++] = 0x24502550;
  s->store[i++] = 0x26501956;
  s->store[i++] = 0x19671968;
  s->store[i++] = 0x19232042;
  s->store[i++] = 0x30424042;
  s->store[i++] = 0x50426042;
  s->store[i++] = 0x20783078;
  s->store[i++] = 0x40785078;
  s->store[i++] = 0x60782142;
  s->store[i++] = 0x31424142;
  s->store[i++] = 0x51426142;
  s->store[i++] = 0x21783178;
  s->store[i++] = 0x41785178;
  s->store[i++] = 0x61781490;
  s->store[i++] = 0x14621445;
  s->store[i++] = 0x14271409;
  s->store[i++] = 0x70232425;
  s->store[i++] = 0x80232425;
  s->store[i++] = 0x90232425;
  s->store[i++] = 0x79777879;
  s->store[i++] = 0x89777879;
  s->store[i++] = 0x99777879;
  s->store[i++] = 0x71232425;
  s->store[i++] = 0x74507778;
  s->store[i++] = 0x75507778;
  s->store[i++] = 0x76507778;
  s->store[i++] = 0x84507778;
  s->store[i++] = 0x85507778;
  s->store[i++] = 0x86507778;
}

