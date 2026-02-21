/**
 * @file classJRTRelayBoard.ino
 * @brief JRTで使用する新中継基板のプログラム
 * @author ka-futatsui
 * @date 2025/11/10 
 */

#include "classJRTRelayBoard.h"
#include <WiFi.h>

/**
     * @brief JRTRelayBoardの唯一のインスタンスへの参照を取得する
     * @return JRTRelayBoardクラスのインスタンスへの参照
     * * NOTE: ポインタではなく参照を返し、静的ローカル変数で安全なシングルトンを実現
     */
classJRTRelayBoard& classJRTRelayBoard::getInstance(void) {
  // 関数が最初に呼び出されたときに一度だけ初期化される
  // プログラム終了時に自動的に破棄されるため、メモリリークの心配がない
  static classJRTRelayBoard instance;
  return instance;
}

/**
 * @brief classJRTRelayBoardのコンストラクタ
 * @detail 初期設定をここで行う
 */
classJRTRelayBoard::classJRTRelayBoard() {
  // ピンの初期設定
  pinMode(msc_u8LedPin, OUTPUT);  // デジタルピン2をOutput指定

  // WiFiを無効化
  WiFi.mode(WIFI_OFF);

  // Bluetoothを無効化
  btStop();

  // ピンモードを設定
  /// ディジタル入力ピン設定
  pinMode(msc_u8DigitalInput1, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput2, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput3, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput4, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput5, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput6, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput7, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput8, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput9, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput10, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput11, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput12, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput13, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput14, INPUT_PULLUP);
  pinMode(msc_u8DigitalInput15, INPUT_PULLUP);
  /// アナログ入力ピン設定
  pinMode(msc_u8AnalogInput1, ANALOG);
  pinMode(msc_u8AnalogInput2, ANALOG);
  pinMode(msc_u8AnalogInput3, ANALOG);
  pinMode(msc_u8AnalogInput4, ANALOG);
  pinMode(msc_u8AnalogInput5, ANALOG);

  u64LastSendTime = millis();  /// 前回送信時間を現時刻で初期化

  Serial.begin(115200, SERIAL_8N1);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

/**
 * @brief LEDチカチカ（Lチカ）を実行する
 * @note Wi-Fiの機能がOFFの時にLEDが点滅するようになっている
 */
void classJRTRelayBoard::LedFlashing(void) {
  if (WiFi.mode(WIFI_OFF)) {
    digitalWrite(msc_u8LedPin, HIGH);  // LED ON (ピン2：HIGH出力)
    delay(100);                        // 100ms wait
    digitalWrite(msc_u8LedPin, LOW);   // LED OFF (ピン2：LOW出力)
  }
}

/**
     * @brief ディジタル入力端子の状態を取得するための関数
     * @return 処理結果(成功:true,失敗:false)
     */
void classJRTRelayBoard::digitalInputGetStatus(void) {
  m_sStructControllerCommandData.u8DigitalSignalPacket[0] = !digitalRead(msc_u8DigitalInput1) + !digitalRead(msc_u8DigitalInput2) * 2 + !digitalRead(msc_u8DigitalInput3) * 4 + !digitalRead(msc_u8DigitalInput4) * 8 + !digitalRead(msc_u8DigitalInput5) * 16 + !digitalRead(msc_u8DigitalInput6) * 32 + !digitalRead(msc_u8DigitalInput7) * 64 + !digitalRead(msc_u8DigitalInput8) * 128;

  m_sStructControllerCommandData.u8DigitalSignalPacket[1] = !digitalRead(msc_u8DigitalInput9) + !digitalRead(msc_u8DigitalInput10) * 2 + !digitalRead(msc_u8DigitalInput11) * 4 + !digitalRead(msc_u8DigitalInput12) * 8 + !digitalRead(msc_u8DigitalInput13) * 16 + !digitalRead(msc_u8DigitalInput14) * 32 + !digitalRead(msc_u8DigitalInput15) * 64;
}

/**
     * @brief アナログ入力端子の状態を取得するための関数
     * @return 処理結果(成功:true,失敗:false)
     */
void classJRTRelayBoard::analogInputGetStatus(void) {
  m_sStructControllerCommandData.u8AnalogSignalPacket[0] = (uint8_t)((analogRead(msc_u8AnalogInput1) * 255 / 4095));
  m_sStructControllerCommandData.u8AnalogSignalPacket[1] = (uint8_t)((analogRead(msc_u8AnalogInput2) * 255 / 4095));
  m_sStructControllerCommandData.u8AnalogSignalPacket[2] = (uint8_t)((analogRead(msc_u8AnalogInput3) * 255 / 4095));
  m_sStructControllerCommandData.u8AnalogSignalPacket[3] = (uint8_t)((analogRead(msc_u8AnalogInput4) * 255 / 4095));
  m_sStructControllerCommandData.u8AnalogSignalPacket[4] = (uint8_t)((analogRead(msc_u8AnalogInput5) * 255 / 4095));
}

/**
     * @brief 端子の状態を送信するための関数
     * @return 処理結果(成功:true,失敗:false)
     */
bool classJRTRelayBoard::sendInputStatus(void) {
  // 送信バッファを定義（7バイトのデータ * 2桁 + 区切り文字 + \r\n + NULL終端）
  // 30バイトあれば十分と想定
  char pCharCommandBuffer[30];
  uint32_t u32Len = 0;
  if (msc_u64SendcycleTime < (millis() - u64LastSendTime)) {
    u32Len = sprintf(
      pCharCommandBuffer,
      "%02X,%02X,%02X,%02X,%02X,%02X,%02X\r\n",  // データの最後に CR(\r) と LF(\n) を付加
      m_sStructControllerCommandData.u8DigitalSignalPacket[0],
      m_sStructControllerCommandData.u8DigitalSignalPacket[1],
      m_sStructControllerCommandData.u8AnalogSignalPacket[0],
      m_sStructControllerCommandData.u8AnalogSignalPacket[1],
      m_sStructControllerCommandData.u8AnalogSignalPacket[2],
      m_sStructControllerCommandData.u8AnalogSignalPacket[3],
      m_sStructControllerCommandData.u8AnalogSignalPacket[4]);
    //Serial.println(pCharCommandBuffer);
    //Serial.println(u32Len);
    // Serial2.write で、シリアライズされた文字列（長さlen）を送信
    Serial2.write(pCharCommandBuffer, u32Len);

    u64LastSendTime = millis();
    
    Serial.print("u8DigitalSignalPacket[0](D8~D1):");
    Serial.println(m_sStructControllerCommandData.u8DigitalSignalPacket[0], BIN);
    Serial.print("u8DigitalSignalPacket[1](D15~D9):");
    Serial.println(m_sStructControllerCommandData.u8DigitalSignalPacket[1], BIN);
    Serial.print("u8AnalogSignalPacket[0]:");
    Serial.println(m_sStructControllerCommandData.u8AnalogSignalPacket[0]);
    Serial.print("u8AnalogSignalPacket[1]:");
    Serial.println(m_sStructControllerCommandData.u8AnalogSignalPacket[1]);
    Serial.print("u8AnalogSignalPacket[2]:");
    Serial.println(m_sStructControllerCommandData.u8AnalogSignalPacket[2]);
    Serial.print("u8AnalogSignalPacket[3]:");
    Serial.println(m_sStructControllerCommandData.u8AnalogSignalPacket[3]);
    Serial.print("u8AnalogSignalPacket[4]:");
    Serial.println(m_sStructControllerCommandData.u8AnalogSignalPacket[4]);
    
    LedFlashing();
    
    return true;
  }
  return false;
}
