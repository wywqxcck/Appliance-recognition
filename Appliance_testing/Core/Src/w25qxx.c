#include "w25qxx.h"


/**
 * @brief Ƭѡ�ź�
 *
 * @param x
 */
void MySPI_W_SS(uint8_t x)
{
  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, (GPIO_PinState)(x));
}
/**
 * @brief ��ʼ
 *
 */
void MySPI_Start(void)
{
  MySPI_W_SS(0);
}

/**
 * @brief ����
 *
 */
void MySPI_Stop(void)
{
  MySPI_W_SS(1);
}

/**
 * @brief ����һλ����
 *
 * @param ByteSend ���������
 * @return uint8_t ���յ�����
 */
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
  uint8_t Rx;
  HAL_SPI_TransmitReceive(&W25QXX_SPI, &ByteSend, &Rx, 1, 0xffffff);
  return Rx;
}

/**
 * @brief ��������
 *
 * @param pTxData ���������
 * @param pRxData ���յ�����
 * @param Size ��������С
 */
void MySPI_TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  HAL_SPI_TransmitReceive(&W25QXX_SPI, pTxData, pRxData, Size, 0xffffff);
}

/**
 * @brief ��������
 *
 * @param pdata ���������
 * @param Size ��������С
 */
void MySPI_Transmit(uint8_t *pdata, uint16_t Size)
{
  HAL_SPI_Transmit(&W25QXX_SPI, pdata, Size, 0xffffff);
}

/**
 * @brief ����һλ����
 *
 * @param pdata ���������
 */
void MySPI_SendByte(uint8_t pdata)
{
  HAL_SPI_Transmit(&W25QXX_SPI, &pdata, 1, 0xffffff);
}

/**
 * @brief ��������
 *
 * @param pData ���յ�����
 * @param Size ��������С
 */
void MySPI_Receive(uint8_t *pData, uint16_t Size)
{
  HAL_SPI_Receive(&W25QXX_SPI, pData, Size, 0xffffff);
}

/**
 * @brief ��ID
 *
 * @param MID
 * @param DID
 */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
  MySPI_Start();
  MySPI_SendByte(W25Q64_JEDEC_ID);
  *MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
  *DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
  *DID <<= 8;
  *DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
  MySPI_Stop();
}

/**
 * @brief дʹ��
 *
 */
void W25Q64_WriteEnable(void)
{
  MySPI_Start();
  MySPI_SendByte(W25Q64_WRITE_ENABLE);
  MySPI_Stop();
}

/**
 * @brief �ȴ��������
 *
 */
void W25Q64_WaitBusy(void)
{
  uint32_t Timeout;
  MySPI_Start();
  MySPI_SendByte(W25Q64_READ_STATUS_REGISTER_1);
  Timeout = 100000;
  while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
  {
    Timeout--;
    if (Timeout == 0)
    {
      break;
    }
  }
  MySPI_Stop();
}

/**
 * @brief д
 *
 * @param Address ��ַ
 * @param DataArray д�������
 * @param Count ��������С
 */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{

  W25Q64_WriteEnable();

  MySPI_Start();
  MySPI_SendByte(W25Q64_PAGE_PROGRAM);
  MySPI_SendByte(Address >> 16);
  MySPI_SendByte(Address >> 8);
  MySPI_SendByte(Address);

  MySPI_Transmit(DataArray, Count);

  MySPI_Stop();

  W25Q64_WaitBusy();
}

/**
 * @brief ��
 *
 * @param Address ��ַ
 */
void W25Q64_SectorErase(uint32_t Address)
{
  W25Q64_WriteEnable();

  MySPI_Start();
  MySPI_SendByte(W25Q64_SECTOR_ERASE_4KB);
  MySPI_SendByte(Address >> 16);
  MySPI_SendByte(Address >> 8);
  MySPI_SendByte(Address);
  MySPI_Stop();

  W25Q64_WaitBusy();
}

/**
 * @brief ��
 *
 * @param Address ��ַ
 * @param DataArray ����������
 * @param Count ��������С
 */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{

  MySPI_Start();
  MySPI_SendByte(W25Q64_READ_DATA);

  MySPI_SendByte(Address >> 16);
  MySPI_SendByte(Address >> 8);
  MySPI_SendByte(Address);

  MySPI_Receive(DataArray, Count);

  MySPI_Stop();
}


