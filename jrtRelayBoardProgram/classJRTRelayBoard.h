#ifndef JRTRELAYBOARDLIB_H_
#define JRTRELAYBOARDLIB_H_

struct ControllerCommandData {
  uint8_t u8AnalogSignalPacket[5]; ///< 5端子分のアナログ信号を代入するための配列 
  uint8_t u8DigitalSignalPacket[2]; ///< 16端子分のディジタル信号を代入するための配列
};

/**
 * @brief JRTで使用するリレーボードのシングルトンクラス
 * * NOTE: クラス名から冗長な 'class' プレフィックスを削除し、JRTRelayBoardとしています。
 */
class classJRTRelayBoard {
public:

  static classJRTRelayBoard& getInstance(void);

  void LedFlashing(void);

  void digitalInputGetStatus(void);

  void analogInputGetStatus(void);

  bool sendInputStatus(void);

private:
  /**
     * @brief コンストラクタ（外部からの直接生成を禁止）
     */
  classJRTRelayBoard();

  /**
     * @brief コピーコンストラクタと代入演算子を削除し、シングルトン性を保証
     */
  classJRTRelayBoard(const classJRTRelayBoard&) = delete;
  classJRTRelayBoard& operator= (const classJRTRelayBoard&) = delete;


  ControllerCommandData m_sStructControllerCommandData; //< 送信するための構造体のインスタンス

  unsigned long u64LastSendTime;  //< 前回送信時間を記録するための変数

  static const uint8_t msc_u8LedPin = 2; //< ポートを指定するための定数を定義
  static const uint8_t msc_u8DigitalInput1 = 15;
  static const uint8_t msc_u8DigitalInput2 = 4;
  static const uint8_t msc_u8DigitalInput3 = 5;
  static const uint8_t msc_u8DigitalInput4 = 18;
  static const uint8_t msc_u8DigitalInput5 = 19;
  static const uint8_t msc_u8DigitalInput6 = 21;
  static const uint8_t msc_u8DigitalInput7 = 22;
  static const uint8_t msc_u8DigitalInput8 = 23;
  static const uint8_t msc_u8DigitalInput9 = 13;
  static const uint8_t msc_u8DigitalInput10 = 12;
  static const uint8_t msc_u8DigitalInput11 = 14;
  static const uint8_t msc_u8DigitalInput12 = 27;
  static const uint8_t msc_u8DigitalInput13 = 26;
  static const uint8_t msc_u8DigitalInput14 = 25;
  static const uint8_t msc_u8DigitalInput15 = 33;

  static const uint8_t msc_u8AnalogInput1 = 32;
  static const uint8_t msc_u8AnalogInput2 = 35;
  static const uint8_t msc_u8AnalogInput3 = 34;
  static const uint8_t msc_u8AnalogInput4 = 39;
  static const uint8_t msc_u8AnalogInput5 = 36;

  static const unsigned long  msc_u64SendcycleTime = 200;
};

#endif  // #ifndef JRTRELAYBOARDLIB_H_
