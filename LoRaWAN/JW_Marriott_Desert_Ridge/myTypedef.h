//typedef enum myledcolor{
enum myledcolor{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    BLACK
};
//typedef enum SM_STATE{
enum SM_STATE{
    IDLE,
    BLE_COMMISSION,
    LORA_COMMISSION,
    RUNNING
};


int pinPeripheral( uint32_t ulPin, EPioType ulPeripheral );

static __inline__ void syncADC1() __attribute__((always_inline, unused));
static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to);
float BandGap();
uint32_t myanalogRead( uint32_t ulPin );

