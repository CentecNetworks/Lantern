/**
 @file drv_error.h

 @date 2010-02-25

 @version v5.1

 The file contains driver error code definition
*/

#ifndef _DRV_ERROR_H
#define _DRV_ERROR_H

/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/
typedef enum drv_err_e
{
    DRV_E_NONE = 0,

    DRV_E_NO_MEMORY = -10000,
    DRV_E_INVALID_PARAM,          /* -9999 */
    DRV_E_INVALID_ADDR,           /* -9998 */
    DRV_E_FIELD_OVER,             /* -9997 */
    DRV_E_INVALID_TBL,            /* -9996 */
    DRV_E_INVALID_FLD,            /* -9995 */
    DRV_E_INVALID_REG,            /* -9994 */
    DRV_E_INVALID_CHIP,           /* -9993 */
    DRV_E_INVALID_MEM,            /* -9992 */
    DRV_E_INVALID_PTR,            /* -9991 */
    DRV_E_EXCEED_MAX_SIZE,        /* -9990 */
    DRV_E_WRONG_SIZE,             /* -9989 */
    DRV_E_INVALID_DIRECTION,      /* -9988 */
    DRV_E_TCAM_RESET,             /* -9987 */
    DRV_E_NOT_FOUND,              /* -9986 */
    DRV_E_INVAILD_TYPE,           /* -9985 */
    DRV_E_ILLEGAL_LENGTH,         /* -9984 */
    DRV_E_TIME_OUT,               /* -9983 */
    DRV_E_FAIL_CREATE_MUTEX,      /* -9982 */
    DRV_E_INIT_FAILED,            /* -9981 */
    DRV_E_INVALID_ALLOC_INFO,     /* -9980 */
    DRV_E_FILE_OPEN_FAILED,       /* -9979 */
    DRV_E_INVALID_POINT,          /* -9978 */
    DRV_E_INVALID_CHANID,         /* -9977 */
    DRV_ERROR_SEND_CONDITION,     /* -9976 */
    DRV_NOT_FOUND_TBL,            /* -9975 */
    DRV_TBL_ENTRY_SIZE_NOT_MATCH, /* -9974 */
    DRV_TBL_ENTRY_DATA_NOT_MATCH, /* -9973 */
    DRV_CHECK_CMODEL_LINKLIST_IS_NOT_EMPTY, /* -9972 */
    DRV_E_CMODEL_AND_RTL_PKT_NOT_EQUAL,     /* -9971 */
    DRV_E_INVALID_TCAM_TYPE,      /* -9970 */
    DRV_E_FIELD_VALUE_ERROR,      /* -9969 */
    DRV_E_RESERVD_VALUE_ERROR,    /* -9968 */
    DRV_E_CREATE_TIMER,           /* -9967 */
    DRV_E_CREATE_EVENT,           /* -9966 */
    DRV_E_WAIT_EVENT,             /* -9965 */
    DRV_E_INVALID_PARAMETER,      /* -9964 */
    DRV_E_BIT_OUT_RANGE,          /* -9963 */
    DRV_E_STORE_BUS_NO_OPEN_FILE, /* -9962 */
    DRV_E_STORE_BUS_INTER_OPERATION,       /* -9961 */
    DRV_E_TCAM_KEY_DATA_ADDRESS,  /* -9960 */
    DRV_E_TCAM_KEY_MASK_ADDRESS,  /* -9959 */
    DRV_E_TCAM_KEY_CONFIG_ERROR,  /* -9958 */
    DRV_E_INVALID_TCAM_INDEX,     /* -9957 */
    DRV_E_INVALID_TCAM_ALLOC,     /* -9956 */
    DRV_E_INVALID_HASH_ALLOC,     /* -9955 */
    DRV_E_INVALID_DYNIC_TBL_ALLOC,        /* -9954 */
    DRV_E_INVALID_ACTION_TBL_ALLOC,       /* -9953 */
    DRV_E_INVALID_CFG_TBL_ID,    /* -9952 */
    DRV_E_INVALID_CFG_NO_MATCH,  /* -9951 */
    DRV_E_INVALID_CFG_TBL_BASE,  /* -9950 */
    DRV_E_INVALID_HASH_TABLE_SIZE, /* -9949 */
    DRV_E_DATAPATH_CHECK_PLL_LOCK_FAIL, /* -9948*/
    DRV_E_DATAPATH_CHECK_DL_LOCK_FAIL, /* -9947*/
    DRV_E_INVALD_RUNNING_ENV_TYPE, /* -9946*/
    DRV_E_DATAPATH_CHECK_HSS_READY_FAIL, /* -9945*/
    DRV_E_DATAPATH_RELEASE_DLL_LOCK_FAIL, /* -9944*/
    DRV_E_DATAPATH_RESET_SUP_FAIL, /* -9943*/
    DRV_E_DATAPATH_RESET_GLB_FAIL, /* -9942*/
    DRV_E_DATAPATH_RESET_TCAM_CORE_FAIL, /* -9941*/
    DRV_E_DATAPATH_RESET_TCAM_POWER_FAIL, /* -9940*/
    DRV_E_DATAPATH_RELEASE_GLB_FAIL, /* -9939*/
    DRV_E_DATAPATH_RELEASE_SUP_FAIL, /* -9938*/
    DRV_E_DATAPATH_READ_CHIP_FAIL,     /*-9937*/
    DRV_E_DATAPATH_WRITE_CHIP_FAIL, /*-9936*/
    DRV_E_DATAPATH_CLK_ENABLE_FAIL, /*-9935*/
    DRV_E_DATAPATH_INIT_EXT_MEM_FAIL,/*-9934*/
    DRV_E_DATAPATH_INIT_NTSG_SERDES_FAIL,/*-9933*/
    DRV_E_DATAPATH_INIT_UPSG_SERDES_FAIL,/*-9932*/
    DRV_E_DATAPATH_INIT_HSS_FAIL,/*-9931*/
    DRV_E_DATAPATH_INIT_CPU_MAC_FAIL,/*-9930*/
    DRV_E_DATAPATH_INIT_MDIO_FAIL,/*-9929*/
    DRV_E_DATAPATH_RESET_GMAC_FAIL,/*-9928*/
    DRV_E_DATAPATH_INIT_GMAC_FAIL,/*-9927*/
    DRV_E_DATAPATH_INIT_FABRIC_FAIL,/*-9926*/
    DRV_E_DATAPATH_INIT_MEM_FAIL,/*-9925*/
    DRV_E_DATAPATH_INIT_DATAPATH_FAIL,/*-9924*/
    DRV_E_DATAPATH_RESET_FOAM_GLB_FAIL,/*-9923*/
    DRV_E_DATAPATH_RESET_FOAM_PCI_FAIL,/*-9922*/
    DRV_E_DATAPATH_FILE_NOT_FOUND,            /* -9921 */
    DRV_E_DATAPATH_UPSG_SPEED_ERROR,           /* -9920 */
    DRV_E_DATAPATH_CLK_DISABLE_FAIL,        /*-9919*/
    DRV_E_DATAPATH_PWDN_NTSG_SERDES_FAIL, /*-9918*/
    _DRV_E_LAST
}drv_e_t;

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/

#endif /*end of _DRV_ERROR_H*/

