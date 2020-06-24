/**
  * @file    stm32f429_usbd_cdc.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.9.26
  * @brief   USB OTG 
  */
//--------------------------------------------------------------
//              PB13  -> USB_OTG_VBUS
//              PB12  -> USB_OTG_ID
//              PB14  -> USB_OTG_DM
//              PB15  -> USB_OTG_DP
//--------------------------------------------------------------

#include "stm32f429_usbd_cdc.h"

USB_OTG_CORE_HANDLE  USB_OTG_dev;

extern USB_CDC_STATUS_t USB_CDC_STATUS;


/**
  * @brief  Init vom USB-OTG-Port als CDC-Device
  * @param  None
  * @retval None
  */
void USBD_CDC_Init(void)
{
  USB_CDC_STATUS=USB_CDC_DETACHED;
  USBD_Init(&USB_OTG_dev, USB_OTG_HS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
}

/**
  * @brief  auslesen vom Status der USB-Schnittstelle
  * @param  None
  * @retval USB_CDC_NO_INIT   , USB-Schnittstelle noch nicht initialisiert
            USB_CDC_DETACHED  , USB-Verbindung getrennt
            USB_CDC_CONNECTED , USB-Verbindung hergestellt
  */
USB_CDC_STATUS_t USBD_CDC_GetStatus(void)
{
  return(USB_CDC_STATUS);
}

/**
  * @brief  Sendet einen String per USB-OTG-Schnittstelle
  * @param  end_cmd : [NONE, LFCR, CRLF, LF, CR]
  * @retval ERROR   , wenn String gesendet wurde
            SUCCESS , wenn String nicht gesendet wurde
  */
ErrorStatus USBD_CDC_SendString(char *ptr, USB_CDC_LASTBYTE_t end_cmd)
{

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
  // senden nur, wenn Verbindung hergestellt ist
    return(ERROR);
  }

  // kompletten String senden
  while (*ptr != 0) {
    VCP_DataTx(*ptr);
    ptr++;
  }
  // eventuell Endekennung senden
  if(end_cmd==LFCR) {
    VCP_DataTx(0x0A); // LineFeed senden
    VCP_DataTx(0x0D); // CariageReturn senden
  }
  else if(end_cmd==CRLF) {
    VCP_DataTx(0x0D); // CariageReturn senden
    VCP_DataTx(0x0A); // LineFeed senden
  }
  else if(end_cmd==LF) {
    VCP_DataTx(0x0A); // LineFeed senden
  }
  else if(end_cmd==CR) {
    VCP_DataTx(0x0D); // CariageReturn senden
  }

  return(SUCCESS);
}


/**
  * @brief  einen String per USB-OTG-Schnittstelle empfangen
            (der Empfang wird per Interrupt abgehandelt)
             diese Funktion muss zyklisch gepollt werden
  * @param  String steht in *ptr
  * @retval wenn USB nicht bereit = RX_USB_ERR
            wenn nichts empfangen = RX_EMPTY
            wenn String empfangen = RX_READY 
  */
USB_CDC_RXSTATUS_t USBD_CDC_ReceiveString(char *ptr)
{
  uint16_t check;

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
    // empfangen nur, wenn Verbindung hergestellt ist
    return(RX_USB_ERR);
  }

  check=VCP_StringRx(ptr);
  if(check==0) {
    ptr[0]=0x00;
    return(RX_EMPTY);
  }

  return(RX_READY);
}
