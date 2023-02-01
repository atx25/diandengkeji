
#define BLINKER_WIFI
#define TFT_MOSI PIN_D7
#define TFT_SCLK PIN_D5

#include <Blinker.h>
#include <TFT_eSPI.h>
#include <font_24.h>
TFT_eSPI tft = TFT_eSPI(240,240);
TFT_eSprite clk=TFT_eSprite(&tft);
int f=0;
//设定屏幕大小


char auth[] = "7e534c512c95";  
char ssid[] = "HUAWEI-0409E6"; 
char pswd[] = "15660090095";
// 新建组件对象

BlinkerNumber Number1("num-abc");
BlinkerNumber Ntem("num-tem");
BlinkerNumber Nhum("num-hum");
BlinkerNumber HUMI("humi"); //数据组件名
BlinkerNumber TEMP("temp");
BlinkerText tex("tex-qqp");
char Rxbyte[3];
char Txbyte[1];
void Sendbyte(char Tx[])
{

    Serial.write(Tx[0]);
    delay(2);
}
bool Receivebyte()
{
    int i,dn;
    for(i=0;i<3;++i)
    {
        dn=0;
        while(1)
        {
            if(dn<50)
            {
                if(Serial.available()>0)
                {
                    Rxbyte[i]=Serial.read();
                    if(Rxbyte[0]>100||Rxbyte[1]>100){
                        Rxbyte[1]=0;
                        Rxbyte[0]=0;
                    }
                    break;
                }
                else
                {
                    delay(1);
                    dn+=1;
                }
            }
            else return false; 
        }
    }
    return true;
}
//心跳包调试返回数值
void heartbeat()
{
    HUMI.print((int) Rxbyte[0]);  //上传
    TEMP.print((int) Rxbyte[1]);
}
//图表数据抓取
void dataStorage()
{
    Blinker.dataStorage("temp",(int)Rxbyte[0]); //数据组件名，数据值
    Blinker.dataStorage("humi",(int)Rxbyte[1]);
}
//按键1数据提醒与发送

void setup()
{
    
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();
    
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachHeartbeat(heartbeat);
    Blinker.attachDataStorage(dataStorage);
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.loadFont(font_24);
    tft.drawString("温度:", 90, 20);
    tft.drawString("湿度:", 90, 80);
    tft.unloadFont();
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,FUNC_GPIO4);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(4));
}

void loop() {
    Blinker.run();
    if(GPIO_INPUT_GET(GPIO_ID_PIN(4))  == 0){
        tex.print("无异常");
    }
    else{
        tex.print("可燃气体泄漏");
    }
    if(Serial.available()>0)
    {
        if(Receivebyte())
        {
            Ntem.print((int)Rxbyte[0]);
            Nhum.print((int)Rxbyte[1]);
            
            clk.loadFont(font_24);
            clk.setColorDepth(8);
            clk.createSprite(55,25);
            clk.fillSprite(0x0000);
            clk.setTextColor(TFT_BLUE);
            clk.println((int)Rxbyte[0]);
            clk.pushSprite(105,50);  //窗口位置
            clk.deleteSprite();
            clk.unloadFont();
            clk.loadFont(font_24);
            clk.setColorDepth(8);
            clk.createSprite(55,60);
            clk.fillSprite(0x0000);
            clk.setTextColor(TFT_BLUE);
            clk.println((int)Rxbyte[1]);
            clk.pushSprite(105,104);  //窗口位置
            clk.deleteSprite();
            clk.unloadFont(); 
        }
    }
}

