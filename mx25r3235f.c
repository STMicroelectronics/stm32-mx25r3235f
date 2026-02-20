/**
 ******************************************************************************
 * @file    mx25r3235f.c
 * @modify  MCD Application Team
 * @brief   This file provides the MX25R3235F XSPI driver.
 ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "mx25r3235f.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup MX25R3235F MX25R3235F
  * @{
  */

/** @defgroup MX25R3235F_Exported_Functions MX25R3235F Exported Functions
  * @{
  */

/**
  * @brief  Get Flash information
  * @param  pInfo pointer to information structure
  * @retval error status
  */
int32_t MX25R3235F_GetFlashInfo(MX25R3235F_Info_t *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize              = MX25R3235F_FLASH_SIZE;
  pInfo->EraseBlockSize         = MX25R3235F_BLOCK_64K;
  pInfo->EraseBlocksNumber      = (MX25R3235F_FLASH_SIZE/MX25R3235F_BLOCK_64K);
  pInfo->EraseSubBlockSize      = MX25R3235F_BLOCK_32K;
  pInfo->EraseSubBlocksNumber   = (MX25R3235F_FLASH_SIZE/MX25R3235F_BLOCK_32K);
  pInfo->EraseSectorSize        = MX25R3235F_SECTOR_4K;
  pInfo->EraseSectorsNumber     = (MX25R3235F_FLASH_SIZE/MX25R3235F_SECTOR_4K);
  pInfo->ProgPageSize           = MX25R3235F_PAGE_SIZE;
  pInfo->ProgPagesNumber        = (MX25R3235F_FLASH_SIZE/MX25R3235F_PAGE_SIZE);

  return MX25R3235F_OK;
};

/* Read/Write Array Commands ****************************************************/
/**
  * @brief  Reads an amount of data from the XSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval XSPI memory status
  */
int32_t MX25R3235F_Read(XSPI_HandleTypeDef *Ctx, MX25R3235F_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the read command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataLength         = Size;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  switch(Mode)
  {
  case MX25R3235F_SPI_MODE :
    sCommand.Instruction = MX25R3235F_FAST_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_DUAL_OUT_MODE :
    sCommand.Instruction = MX25R3235F_DUAL_OUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_2_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_DUAL_IO_MODE :
    sCommand.Instruction = MX25R3235F_DUAL_INOUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_2_LINES;
    sCommand.DataMode    = HAL_XSPI_DATA_2_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ_DUAL;
    break;

  case MX25R3235F_QUAD_OUT_MODE :
    sCommand.Instruction = MX25R3235F_QUAD_OUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_4_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_QUAD_IO_MODE :
    sCommand.Instruction = MX25R3235F_QUAD_INOUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_4_LINES;
    sCommand.DataMode    = HAL_XSPI_DATA_4_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ_QUAD;
    break;

  default :
    return MX25R3235F_ERROR;
  }

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Writes an amount of data to the XSPI memory.
  *         SPI/QUAD_INOUT/; 1-1-1/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ MX25R3235F_PAGE_SIZE
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval XSPI memory status
  */
int32_t MX25R3235F_PageProgram(XSPI_HandleTypeDef *Ctx, MX25R3235F_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the program command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.Address            = WriteAddr;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataLength         = Size;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  switch(Mode)
  {
  case MX25R3235F_SPI_MODE :
    sCommand.Instruction = MX25R3235F_PAGE_PROG_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
    break;

  case MX25R3235F_QUAD_IO_MODE :
    sCommand.Instruction = MX25R3235F_QUAD_PAGE_PROG_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_4_LINES;
    sCommand.DataMode    = HAL_XSPI_DATA_4_LINES;
    break;

  default :
    return MX25R3235F_ERROR;
  }

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Erases the specified block of the XSPI memory.
  *         MX25R3235F support 4K, 32K and 64K size block erase commands.
  * @param  Ctx Component object pointer
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Block size to erase
  * @retval XSPI memory status
  */
int32_t MX25R3235F_BlockErase(XSPI_HandleTypeDef *Ctx, uint32_t BlockAddress, MX25R3235F_Erase_t BlockSize)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the erase command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.Address            = BlockAddress;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  switch(BlockSize)
  {
  case MX25R3235F_ERASE_4K :
    sCommand.Instruction = MX25R3235F_SECTOR_ERASE_CMD;
    break;

  case MX25R3235F_ERASE_32K :
    sCommand.Instruction = MX25R3235F_SUBBLOCK_ERASE_CMD;
    break;

  case MX25R3235F_ERASE_64K :
    sCommand.Instruction = MX25R3235F_BLOCK_ERASE_CMD;
    break;

  default :
    return MX25R3235F_ERROR;
  }

  /* Send the command */
  if(HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Whole chip erase.
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval error status
  */
int32_t MX25R3235F_ChipErase(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the erase command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_CHIP_ERASE_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if(HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Enable memory mapped mode for the XSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval XSPI memory status
  */
int32_t MX25R3235F_EnableMemoryMappedMode(XSPI_HandleTypeDef *Ctx, MX25R3235F_Interface_t Mode)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  XSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};

  /* Initialize the read command */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  switch(Mode)
  {
  case MX25R3235F_SPI_MODE :
    sCommand.Instruction = MX25R3235F_FAST_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_DUAL_OUT_MODE :
    sCommand.Instruction = MX25R3235F_DUAL_OUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_2_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_DUAL_IO_MODE :
    sCommand.Instruction = MX25R3235F_DUAL_INOUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_2_LINES;
    sCommand.DataMode    = HAL_XSPI_DATA_2_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ_DUAL;
    break;

  case MX25R3235F_QUAD_OUT_MODE :
    sCommand.Instruction = MX25R3235F_QUAD_OUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
    sCommand.DataMode    = HAL_XSPI_DATA_4_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ;
    break;

  case MX25R3235F_QUAD_IO_MODE :
    sCommand.Instruction = MX25R3235F_QUAD_INOUT_READ_CMD;
    sCommand.AddressMode = HAL_XSPI_ADDRESS_4_LINES;
    sCommand.DataMode    = HAL_XSPI_DATA_4_LINES;
    sCommand.DummyCycles = DUMMY_CYCLES_READ_QUAD;
    break;

  default :
    return MX25R3235F_ERROR;
  }

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }


  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_XSPI_MemoryMapped(Ctx, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Flash suspend program or erase command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_Suspend(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the suspend command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_PROG_ERASE_SUSPEND_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Flash resume program or erase command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_Resume(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the resume command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_PROG_ERASE_RESUME_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/* Register/Setting Commands **************************************************/
/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_WriteEnable(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef     sCommand = {0};

  /* Initialize the write enable command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_ENABLE_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  sCommand.Instruction = MX25R3235F_READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;

  return MX25R3235F_OK;
}

/**
  * @brief  This function reset the (WEN) Write Enable Latch bit.
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_WriteDisable(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the write disable command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_DISABLE_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Read Flash Status register value
  * @param  Ctx Component object pointer
  * @param  Value Status register value pointer
  * @retval error status
  */
int32_t MX25R3235F_ReadStatusRegister(XSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the reading of status register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_READ_STATUS_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 1U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, Value, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Write Flash Status register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to Status register
  * @retval error status
  */
int32_t MX25R3235F_WriteStatusRegister(XSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  uint8_t reg[3];

  /* Status register is configured with configuration register 1 and 2 */
  if (MX25R3235F_ReadCfgRegister(Ctx, &reg[1]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  if (MX25R3235F_ReadCfg2Register(Ctx, &reg[2]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  reg[0] = Value;

  /* Initialize the writing of status register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_STATUS_CFG_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 3U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Write Flash configuration register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to configuration register
  * @retval error status
  */
int32_t MX25R3235F_WriteCfgRegister(XSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  uint8_t reg[3];

  /* Configuration register is configured with configuration register 2 and status register */
  if (MX25R3235F_ReadStatusRegister(Ctx, &reg[0]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  if (MX25R3235F_ReadCfg2Register(Ctx, &reg[2]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  reg[1] = Value;

  /* Initialize the writing of configuration register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_STATUS_CFG_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 3U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Read Flash configuration register value
  * @param  Ctx Component object pointer
  * @param  Value configuration register value pointer
  * @retval error status
  */
int32_t MX25R3235F_ReadCfgRegister(XSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  uint8_t reg[2];

  /* Initialize the reading of configuration register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_READ_CFG_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 2U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  *Value = reg[0];

  return MX25R3235F_OK;
}

/**
  * @brief  Write Flash configuration register 2
  * @param  Ctx Component object pointer
  * @param  Value Value to write to configuration register
  * @retval error status
  */
int32_t MX25R3235F_WriteCfg2Register(XSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  uint8_t reg[3];

  /* Configuration register 2 is configured with configuration register 1 and status register */
  if (MX25R3235F_ReadStatusRegister(Ctx, &reg[0]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  if (MX25R3235F_ReadCfg2Register(Ctx, &reg[1]) != MX25R3235F_OK)
  {
    return MX25R3235F_ERROR;
  }

  reg[2] = Value;

  /* Initialize the writing of configuration register 2 */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_STATUS_CFG_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 3U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Read Flash configuration register 2 value
  * @param  Ctx Component object pointer
  * @param  Value configuration register 2 value pointer
  * @retval error status
  */
int32_t MX25R3235F_ReadCfg2Register(XSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};
  uint8_t reg[2];

  /* Initialize the reading of status register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_READ_CFG_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 2U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  *Value = reg[1];

  return MX25R3235F_OK;
}

/**
  * @brief  Write Flash Security register
  * @param  Ctx Component object pointer
  * @param  Value Value to write to Security register
  * @retval error status
  */
int32_t MX25R3235F_WriteSecurityRegister(XSPI_HandleTypeDef *Ctx, uint8_t Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the write of security register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_WRITE_SEC_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 1U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, &Value, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Read Flash Security register value
  * @param  Ctx Component object pointer
  * @param  Value Security register value pointer
  * @retval error status
  */
int32_t MX25R3235F_ReadSecurityRegister(XSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the reading of security register */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_READ_SEC_REG_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 1U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, Value, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}


/* ID Commands ****************************************************************/
/**
  * @brief  Read Flash 3 Byte IDs.
  *         Manufacturer ID, Memory type, Memory density
  * @param  Ctx Component object pointer
  * @param  ID 3 bytes IDs pointer
  * @retval error status
  */
int32_t MX25R3235F_ReadID(XSPI_HandleTypeDef *Ctx, uint8_t *ID)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the read ID command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_READ_ID_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataLength         = 3U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, ID, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/* Reset Commands *************************************************************/
/**
  * @brief  Flash reset enable command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_ResetEnable(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the reset enable command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_RESET_ENABLE_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Flash reset memory command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_ResetMemory(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the reset command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_RESET_MEMORY_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Flash no operation command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_NoOperation(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the no operation command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_NO_OPERATION_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @brief  Flash enter deep power-down command
  * @param  Ctx Component object pointer
  * @retval error status
  */
int32_t MX25R3235F_EnterPowerDown(XSPI_HandleTypeDef *Ctx)
{
  XSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize the enter power down command */
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = MX25R3235F_DEEP_POWER_DOWN_CMD;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25R3235F_ERROR;
  }

  return MX25R3235F_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
