/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal88XXPwrSeqCmd.c
	
Abstract:
	Implement HW Power sequence configuration CMD handling routine for Realtek devices.
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2012-04-03 Filen            Create.	
--*/

#include "../HalPrecomp.h"

//
//	Description: 
//		This routine deal with the Power Configuration CMDs parsing for RTL8723/RTL8188E Series IC.
//
//	Assumption:
//		We should follow specific format which was released from HW SD. 
//
//	2011.07.07, added by Roger.
//
BOOLEAN
HalPwrSeqCmdParsing88XX(
    IN      HAL_PADAPTER        Adapter,
    IN      u1Byte              CutVersion, 
    IN      u1Byte              FabVersion, 
    IN      u1Byte              InterfaceType, 
    IN      WLAN_PWR_CFG        PwrSeqCmd[]
)
{
		
	WLAN_PWR_CFG        PwrCfgCmd = {0};
    u32                 bPollingBit = _FALSE;
    u32                 AryIdx=0;
    u8                  value = 0;
    u32                 offset = 0;
    u32                 pollingCount = 0; // polling autoload done.
    u32                 maxPollingCnt = 5000;

	do {
		PwrCfgCmd=PwrSeqCmd[AryIdx];

        RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, ENTRY, offset:0x%x, cut_msk:0x%x, fab_msk:0x%x, if_msk:0x%x, base:0x%x, cmd:0x%x, msk:0x%x, value:0x%x\n", 
    			__FUNCTION__, __LINE__, GET_PWR_CFG_OFFSET(PwrCfgCmd), GET_PWR_CFG_CUT_MASK(PwrCfgCmd), 
    			GET_PWR_CFG_FAB_MASK(PwrCfgCmd), GET_PWR_CFG_INTF_MASK(PwrCfgCmd), GET_PWR_CFG_BASE(PwrCfgCmd), 
    			GET_PWR_CFG_CMD(PwrCfgCmd), GET_PWR_CFG_MASK(PwrCfgCmd), GET_PWR_CFG_VALUE(PwrCfgCmd)) );


		//2 Only Handle the command whose FAB, CUT, and Interface are matched
		if((GET_PWR_CFG_FAB_MASK(PwrCfgCmd)&FabVersion)&&
			(GET_PWR_CFG_CUT_MASK(PwrCfgCmd)&CutVersion)&&
			(GET_PWR_CFG_INTF_MASK(PwrCfgCmd)&InterfaceType)) {
			switch(GET_PWR_CFG_CMD(PwrCfgCmd))
			{
			case PWR_CMD_READ:
                RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, PWR_CMD_READ\n", __FUNCTION__, __LINE__));
				break;

			case PWR_CMD_WRITE:
				RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, PWR_CMD_WRITE\n", __FUNCTION__, __LINE__));
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);

#ifdef CONFIG_SDIO_HCI
				//
				// <Roger_Notes> We should deal with interface specific address mapping for some interfaces, e.g., SDIO interface
				// 2011.07.07.
				//
				if (GET_PWR_CFG_BASE(PwrCfgCmd) == PWR_BASEADDR_SDIO)
				{
					// Read Back SDIO Local value
					value = SdioLocalCmd52Read1Byte(Adapter, offset);

					value &= ~(GET_PWR_CFG_MASK(PwrCfgCmd));
					value |= (GET_PWR_CFG_VALUE(PwrCfgCmd) & GET_PWR_CFG_MASK(PwrCfgCmd));

					// Write Back SDIO Local value
					SdioLocalCmd52Write1Byte(Adapter, offset, value);
				}
				else
#endif
				{
					//Read the value from system register		
					value = HAL_RTL_R8(offset);
					value = value&(~(GET_PWR_CFG_MASK(PwrCfgCmd)));
					value = value|(GET_PWR_CFG_VALUE(PwrCfgCmd)&GET_PWR_CFG_MASK(PwrCfgCmd));

					//Write the value back to sytem register
					HAL_RTL_W8(offset, value);
				}
				break;

			case PWR_CMD_POLLING:
				RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, PWR_CMD_POLLING\n", __FUNCTION__, __LINE__));
				bPollingBit = _FALSE;					
				offset      = GET_PWR_CFG_OFFSET(PwrCfgCmd);				

				do {
#ifdef CONFIG_SDIO_HCI
					if (GET_PWR_CFG_BASE(PwrCfgCmd) == PWR_BASEADDR_SDIO)
						value = SdioLocalCmd52Read1Byte(Adapter, offset);
					else
#endif
						value = HAL_RTL_R8(offset);
					value = value & GET_PWR_CFG_MASK(PwrCfgCmd);
					if(value==(GET_PWR_CFG_VALUE(PwrCfgCmd)&GET_PWR_CFG_MASK(PwrCfgCmd)))
						bPollingBit = _TRUE;
					else	
						HAL_delay_us(10);
					
                    if(pollingCount++ > maxPollingCnt)
                    {
                        printk("cliW PWR_CMD_POLLING, Fail to polling Offset[0x%x]\n", offset);
                        
                        printk("[0xF0-0xF3] = [%02x %02x %02x %02x]\n"
                            , HAL_RTL_R8(0xF0), HAL_RTL_R8(0xF1), HAL_RTL_R8(0xF2), HAL_RTL_R8(0xF3));
                        
                        printk("[0x00-0x07] = [%02x %02x %02x %02x %02x %02x %02x %02x]\n"
                            , HAL_RTL_R8(0x00), HAL_RTL_R8(0x01), HAL_RTL_R8(0x02), HAL_RTL_R8(0x03)
                            , HAL_RTL_R8(0x04), HAL_RTL_R8(0x05), HAL_RTL_R8(0x06), HAL_RTL_R8(0x07));

                        printk("[0x100-0x103] = [%02x %02x %02x %02x]\n"
                            , HAL_RTL_R8(0x100), HAL_RTL_R8(0x101), HAL_RTL_R8(0x102), HAL_RTL_R8(0x103));

                        value = HAL_RTL_R8(0xF6);
                        value &= (0xFC);
                        value |= (0x01);
                        HAL_RTL_W8(0xF6, value);

                        HAL_RTL_W8(0x3A, 0x07);
                        value = HAL_RTL_R8(0xC0);

                        HAL_RTL_W8(0x3A, 0x01);
                        printk("[0x3A = 0x07, 0xC0 = %02x][0x3A = 0x01, 0xC0-0xC3 = %02x %02x %02x %02x]\n"
                            , value, HAL_RTL_R8(0xC0), HAL_RTL_R8(0xC1), HAL_RTL_R8(0xC2), HAL_RTL_R8(0xC3));

                        value = HAL_RTL_R8(0xA3);
                        value &= (0xF8);
                        value |= (0x04);
                        HAL_RTL_W8(0xA3, value);

                        printk("[0xA0-0xA1] = [%02x %02x] \n", HAL_RTL_R8(0xA0), HAL_RTL_R8(0xA1));
                        RT_TRACE(COMP_INIT, DBG_WARNING, ("%s %d, PWR_CMD_POLLING, Fail to polling Offset[0x%x]\n", __FUNCTION__, __LINE__, offset));
						return _FALSE;
					}
				}while(!bPollingBit);

				break;

			case PWR_CMD_DELAY:
                RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, PWR_CMD_DELAY\n", __FUNCTION__, __LINE__));
				if(GET_PWR_CFG_VALUE(PwrCfgCmd) == PWRSEQ_DELAY_US)
					HAL_delay_us(GET_PWR_CFG_OFFSET(PwrCfgCmd));
				else
					HAL_delay_us(GET_PWR_CFG_OFFSET(PwrCfgCmd)*1000);
				break;

			case PWR_CMD_END:
				// When this command is parsed, end the process
                RT_TRACE(COMP_INIT, DBG_LOUD, ("%s %d, PWR_CMD_END\n", __FUNCTION__, __LINE__));
				return _TRUE;
				break;

			default:
                RT_TRACE(COMP_INIT, DBG_SERIOUS, ("%s %d, Unknown CMD!!\n", __FUNCTION__, __LINE__));
				break;
			}

		}
		
		AryIdx++;//Add Array Index
	}while(1);

	return _TRUE;
}

