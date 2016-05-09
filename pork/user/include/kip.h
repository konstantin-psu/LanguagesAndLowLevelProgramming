
struct KernelInterface {
  char     magic[4];
  unsigned apiVersion;
  unsigned apiFlags;
  unsigned kernDescPtr;
  unsigned pad1[17];
  unsigned memoryInfo;
  unsigned pad2[19];
  unsigned virtRegInfo;
  unsigned utcbInfo;
  unsigned kipAreaInfo;
  unsigned pad3[2];
  unsigned bootInfo;
  unsigned procDescPtr;
  unsigned clockInfo;
  unsigned threadInfo;
  unsigned pageInfo;
  unsigned processorInfo;
  unsigned scSpaceControl;
  unsigned scThreadControl;
  unsigned scProcessorControl;
  unsigned scMemoryControl;
  unsigned scIpc;
  unsigned scLipc;
  unsigned scUnmap;
  unsigned scExchangeRegisters;
  unsigned pad4[1];
  unsigned scThreadSwitch;
  unsigned scSchedule;
  unsigned pad5[1];
};

struct KernelDesc {
  unsigned id;
  unsigned gendate;
  unsigned ver;
  char     supplier[4];
  char     features[0];
};

struct MemDesc {
  unsigned hi;
  unsigned lo;
};

struct ProcDesc {
  unsigned externalFreq;
  unsigned internalFreq;
};

