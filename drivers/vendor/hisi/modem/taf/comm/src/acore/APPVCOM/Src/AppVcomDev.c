

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "PsTypeDef.h"
#include "vos.h"
#include "PsCommonDef.h"
#include "AppVcomDev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_APP_VCOM_DEV_C
/*lint -e767 */

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/* VCOM CTX,用于保存VCOM的全局变量*/
APP_VCOM_DEV_CTX_STRU                   g_astVcomCtx[APP_VCOM_MAX_NUM];

APP_VCOM_DEBUG_INFO_STRU                g_stAppVcomDebugInfo;



/* 虚拟串口文件操作接口 */
#if (VOS_OS_VER == VOS_WIN32)
static struct file_operations           g_stOperations_Fops;
#else
static const struct file_operations     g_stOperations_Fops =
{
    .owner = THIS_MODULE,
    .read  = APP_VCOM_Read,
    .poll  = APP_VCOM_Poll,
    .write = APP_VCOM_Write,
    .open  = APP_VCOM_Open,
    .release = APP_VCOM_Release,
};
#endif

VOS_UINT32                              g_ulAppVcomDebugFlg = 0;
#if (FEATURE_ON == MBB_WPG_COMMON)
static VOS_UINT32                       g_ulAppVcomDisableFlg = 0;
#endif


/* APPVCOM的规格和应用
APPVCOM ID   缓存大小  用途           是否AT的CLIENT    ModemId
APPVCOM        4K      RIL                   是         MODEM0
APPVCOM1       4K      RIL                   是         MODEM0
APPVCOM2       4K      工程菜单              是         MODEM0
APPVCOM3       8K      生产装备(AT SERVER)   是         MODEM0
APPVCOM4       4K      audio RIL             是         MODEM0
APPVCOM5       4K      RIL                   是         MODEM1
APPVCOM6       4K      RIL                   是         MODEM1
APPVCOM7       8K      生产装备(AT SERVER)   是         MODEM1
APPVCOM8       4K      工程菜单              是         MODEM1
APPVCOM9       4K      AGPS                  是         MODEM0
APPVCOM10      4K      NFC/BIP               是         MODEM0
APPVCOM11      4K      ISDB                  是         MODEM0
APPVCOM12      4K      audio ril             是         MODEM1
APPVCOM13      4K      SIM                   是         MODEM0
APPVCOM14      4K      SIM                   是         MODEM1
APPVCOM15      4K      预留                  是         MODEM0
APPVCOM16      4K      预留                  是         MODEM0
APPVCOM17      4K      预留                  是         MODEM0
APPVCOM18      4K      预留                  是         MODEM0
APPVCOM19      4K      预留                  是         MODEM0
APPVCOM20      4K      预留                  是         MODEM0
APPVCOM21      4K      预留                  是         MODEM0
APPVCOM22      4K      预留                  是         MODEM0
APPVCOM23      4K      预留                  是         MODEM0
APPVCOM24      4K      预留                  是         MODEM0
APPVCOM25      4K      预留                  是         MODEM0
APPVCOM26      4K      预留                  是         MODEM0
APPVCOM27      2M      CBT                   是         MODEM0
APPVCOM28      4K      T/L装备               否
APPVCOM29      16K     errlog                否
APPVCOM30      2M      log 3.5               否
APPVCOM31      2M      log 3.5               否
*/
#if (FEATURE_ON == FEATURE_VCOM_EXT)
const APP_VCOM_DEV_CONFIG_STRU g_astAppVcomCogfigTab[] =
{
#if (FEATURE_ON == MBB_WPG_COMMON)
    {APP_VCOM_DEV_NAME_0, APP_VCOM_SEM_NAME_0, 0x4000},                         /* APPVCOM */
    {APP_VCOM_DEV_NAME_1, APP_VCOM_SEM_NAME_1, 0x4000},                         /* APPVCOM1 */
#else
    {APP_VCOM_DEV_NAME_0, APP_VCOM_SEM_NAME_0, 0x1000},                         /* APPVCOM */
    {APP_VCOM_DEV_NAME_1, APP_VCOM_SEM_NAME_1, 0x1000},                         /* APPVCOM1 */
#endif
    {APP_VCOM_DEV_NAME_2, APP_VCOM_SEM_NAME_2, 0x1000},                         /* APPVCOM2 */
    {APP_VCOM_DEV_NAME_3, APP_VCOM_SEM_NAME_3, 0x2000},                         /* APPVCOM3 */
    {APP_VCOM_DEV_NAME_4, APP_VCOM_SEM_NAME_4, 0x1000},                         /* APPVCOM4 */
    {APP_VCOM_DEV_NAME_5, APP_VCOM_SEM_NAME_5, 0x1000},                         /* APPVCOM5 */
    {APP_VCOM_DEV_NAME_6, APP_VCOM_SEM_NAME_6, 0x1000},                         /* APPVCOM6 */
    {APP_VCOM_DEV_NAME_7, APP_VCOM_SEM_NAME_7, 0x2000},                         /* APPVCOM7 */
    {APP_VCOM_DEV_NAME_8, APP_VCOM_SEM_NAME_8, 0x1000},                         /* APPVCOM8 */
    {APP_VCOM_DEV_NAME_9, APP_VCOM_SEM_NAME_9, 0x1000},                         /* APPVCOM9 */
    {APP_VCOM_DEV_NAME_10, APP_VCOM_SEM_NAME_10, 0x1000},                       /* APPVCOM10 */
    {APP_VCOM_DEV_NAME_11, APP_VCOM_SEM_NAME_11, 0x1000},                       /* APPVCOM11 */
    {APP_VCOM_DEV_NAME_12, APP_VCOM_SEM_NAME_12, 0x1000},                       /* APPVCOM12 */
    {APP_VCOM_DEV_NAME_13, APP_VCOM_SEM_NAME_13, 0x1000},                       /* APPVCOM13 */
    {APP_VCOM_DEV_NAME_14, APP_VCOM_SEM_NAME_14, 0x1000},                       /* APPVCOM14 */
    {APP_VCOM_DEV_NAME_15, APP_VCOM_SEM_NAME_15, 0x1000},                       /* APPVCOM15 */
    {APP_VCOM_DEV_NAME_16, APP_VCOM_SEM_NAME_16, 0x1000},                       /* APPVCOM16 */
    {APP_VCOM_DEV_NAME_17, APP_VCOM_SEM_NAME_17, 0x1000},                       /* APPVCOM17 */
    {APP_VCOM_DEV_NAME_18, APP_VCOM_SEM_NAME_18, 0x1000},                       /* APPVCOM18 */
    {APP_VCOM_DEV_NAME_19, APP_VCOM_SEM_NAME_19, 0x1000},                       /* APPVCOM19 */
    {APP_VCOM_DEV_NAME_20, APP_VCOM_SEM_NAME_20, 0x1000},                       /* APPVCOM20 */
    {APP_VCOM_DEV_NAME_21, APP_VCOM_SEM_NAME_21, 0x1000},                       /* APPVCOM21 */
    {APP_VCOM_DEV_NAME_22, APP_VCOM_SEM_NAME_22, 0x1000},                       /* APPVCOM22 */
    {APP_VCOM_DEV_NAME_23, APP_VCOM_SEM_NAME_23, 0x1000},                       /* APPVCOM23 */
    {APP_VCOM_DEV_NAME_24, APP_VCOM_SEM_NAME_24, 0x1000},                       /* APPVCOM24 */
    {APP_VCOM_DEV_NAME_25, APP_VCOM_SEM_NAME_25, 0x1000},                       /* APPVCOM25 */
    {APP_VCOM_DEV_NAME_26, APP_VCOM_SEM_NAME_26, 0x1000},                       /* APPVCOM26 */
    {APP_VCOM_DEV_NAME_27, APP_VCOM_SEM_NAME_27, 0x200000},                     /* APPVCOM27 */
    {APP_VCOM_DEV_NAME_28, APP_VCOM_SEM_NAME_28, 0x1000},                       /* APPVCOM28 */
    {APP_VCOM_DEV_NAME_29, APP_VCOM_SEM_NAME_29, 0x4000},                       /* APPVCOM29 */
    {APP_VCOM_DEV_NAME_30, APP_VCOM_SEM_NAME_30, 0x200000},                     /* APPVCOM30 */
    {APP_VCOM_DEV_NAME_31, APP_VCOM_SEM_NAME_31, 0x200000}                      /* APPVCOM31 */
};
#else
const APP_VCOM_DEV_CONFIG_STRU g_astAppVcomCogfigTab[] =
{
#if (FEATURE_ON == MBB_WPG_COMMON)
    {APP_VCOM_DEV_NAME_0, APP_VCOM_SEM_NAME_0, 0x4000},                         /* APPVCOM */
    {APP_VCOM_DEV_NAME_1, APP_VCOM_SEM_NAME_1, 0x4000}                          /* APPVCOM1 */
#else
    {APP_VCOM_DEV_NAME_0, APP_VCOM_SEM_NAME_0, 0x1000},                         /* APPVCOM */
    {APP_VCOM_DEV_NAME_1, APP_VCOM_SEM_NAME_1, 0x1000}                          /* APPVCOM1 */
#endif
#if 0
    {APP_VCOM_DEV_NAME_2, APP_VCOM_SEM_NAME_2, 0x1000},                         /* APPVCOM2 */
    {APP_VCOM_DEV_NAME_3, APP_VCOM_SEM_NAME_3, 0x2000},                         /* APPVCOM3 */
    {APP_VCOM_DEV_NAME_4, APP_VCOM_SEM_NAME_4, 0x1000}                          /* APPVCOM4 */
#endif
};
#endif
/*****************************************************************************
   3 函数、变量声明
*****************************************************************************/

/*****************************************************************************
   4 函数实现
*****************************************************************************/

APP_VCOM_DEV_CTX_STRU* APP_VCOM_GetVcomCtxAddr(VOS_UINT8 ucIndex)
{
    return &(g_astVcomCtx[ucIndex]);
}


APP_VCOM_DEV_ENTITY_STRU* APP_VCOM_GetAppVcomDevEntity(VOS_UINT8 ucIndex)
{
    return (g_astVcomCtx[ucIndex].pstAppVcomDevEntity);
}


VOS_UINT32 APP_VCOM_RegDataCallback(VOS_UINT8 ucDevIndex, SEND_UL_AT_FUNC pFunc)
{
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    /* 索引号错误*/
    if (ucDevIndex >= APP_VCOM_DEV_INDEX_BUTT)
    {
        return VOS_ERR;
    }

    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucDevIndex);

    /* 函数指针赋给全局变量*/
    pstVcomCtx->pSendUlAtFunc = pFunc;

    return VOS_OK;
}


VOS_UINT32 APP_VCOM_RegEvtCallback(VOS_UINT8 ucDevIndex, EVENT_FUNC pFunc)
{
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    /* 索引号错误*/
    if (ucDevIndex >= APP_VCOM_DEV_INDEX_BUTT)
    {
        return VOS_ERR;
    }

    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucDevIndex);

    /* 函数指针赋给全局变量*/
    pstVcomCtx->pEventFunc = pFunc;
#endif
    return VOS_OK;
}


VOS_UINT8 APP_VCOM_GetIndexFromMajorDevId(VOS_UINT ulMajorDevId)
{
    VOS_UINT8                           ucLoop;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    for (ucLoop = 0; ucLoop < APP_VCOM_MAX_NUM; ucLoop++)
    {
        pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucLoop);

        if (pstVcomCtx->ulAppVcomMajorId == ulMajorDevId)
        {
            break;
        }
    }

    return ucLoop;
}


VOS_VOID  APP_VCOM_InitSpecCtx(VOS_UINT8 ucDevIndex)
{
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    if (ucDevIndex >= APP_VCOM_MAX_NUM)
    {
        return;
    }

    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucDevIndex);

    VOS_MemSet(pstVcomCtx->aucSendSemName, 0, APP_VCOM_SEM_NAME_MAX_LEN);
    VOS_MemSet(pstVcomCtx->aucAppVcomName, 0, APP_VCOM_DEV_NAME_MAX_LEN);

    VOS_MemCpy(pstVcomCtx->aucAppVcomName,
               g_astAppVcomCogfigTab[ucDevIndex].pcAppVcomName,
               VOS_StrLen(g_astAppVcomCogfigTab[ucDevIndex].pcAppVcomName));

    VOS_MemCpy(pstVcomCtx->aucSendSemName,
               g_astAppVcomCogfigTab[ucDevIndex].pcSendSemName,
               VOS_StrLen(g_astAppVcomCogfigTab[ucDevIndex].pcSendSemName));

    pstVcomCtx->ulAppVcomMajorId = APP_VCOM_MAJOR_DEV_ID + ucDevIndex;
}


VOS_VOID APP_VCOM_Setup(
    APP_VCOM_DEV_ENTITY_STRU *pstDev,
    VOS_UINT8                 ucIndex
)
{
    VOS_INT                             iErr;
    dev_t                               ulDevno;
    static struct class                *pstCom_class;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

    ulDevno = MKDEV(pstVcomCtx->ulAppVcomMajorId, ucIndex);

    cdev_init(&pstDev->stAppVcomDev, &g_stOperations_Fops);

    iErr = cdev_add(&pstDev->stAppVcomDev, ulDevno, 1);
    if (iErr)
    {
        vos_printf("APP_VCOM_Setup cdev_add error!\n");
        return;
    }

#if (VOS_OS_VER == VOS_WIN32)

#else
    pstCom_class = class_create(THIS_MODULE, pstVcomCtx->aucAppVcomName);

    device_create(pstCom_class,
                  NULL,
                  MKDEV(pstVcomCtx->ulAppVcomMajorId, ucIndex),
                  "%s",
                  pstVcomCtx->aucAppVcomName);
#endif

    return;
}


VOS_INT APP_VCOM_Init(VOS_VOID)
{
    VOS_INT                             iResult1;
    VOS_INT                             iResult2;
    dev_t                               ulDevno;

    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDevp;

    pstVcomCtx = VOS_NULL_PTR;
    pstVcomDevp  = VOS_NULL_PTR;

    /* 初始化可维可测全局变量 */
    VOS_MemSet(&g_stAppVcomDebugInfo, 0x0, sizeof(g_stAppVcomDebugInfo));

    /* 初始化虚拟设备 */
    for (ucIndex = 0; ucIndex < APP_VCOM_MAX_NUM; ucIndex++)
    {
        /* 初始化全局变量 */
        APP_VCOM_InitSpecCtx(ucIndex);

        /* 获取全局变量指针 */
        pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

        /* 将设备号转换成dev_t 类型 */
        ulDevno = MKDEV(pstVcomCtx->ulAppVcomMajorId, ucIndex);

        iResult1 = register_chrdev_region(ulDevno, 1, pstVcomCtx->aucAppVcomName);

        /* 注册失败则动态申请设备号 */
        if (iResult1 < 0)
        {
            iResult2 = alloc_chrdev_region(&ulDevno, 0, 1, pstVcomCtx->aucAppVcomName);

            if (iResult2 < 0 )
            {
                return VOS_ERROR;
            }

            pstVcomCtx->ulAppVcomMajorId = MAJOR(ulDevno);
        }

        /* 动态申请设备结构体内存 */
        pstVcomCtx->pstAppVcomDevEntity = kmalloc(sizeof(APP_VCOM_DEV_ENTITY_STRU) , GFP_KERNEL);

        if (VOS_NULL_PTR == pstVcomCtx->pstAppVcomDevEntity)
        {
            /* 去注册该设备，返回错误 */
            unregister_chrdev_region(ulDevno, 1);
            printk("[VCOM %d]APP_VCOM_Init malloc device Entity fail. \n", ucIndex);
            return VOS_ERROR;
        }

        /* 获取设备实体指针 */
        pstVcomDevp = pstVcomCtx->pstAppVcomDevEntity;

        VOS_MemSet(pstVcomDevp, 0, sizeof(APP_VCOM_DEV_ENTITY_STRU));

        /* Modified by L60609 for add app vcom，2013-06-17,  Begin */
        pstVcomDevp->pucAppVcomMem = kmalloc(g_astAppVcomCogfigTab[ucIndex].ulAppVcomMemSize, GFP_KERNEL);

        if (VOS_NULL_PTR == pstVcomDevp->pucAppVcomMem)
        {
            /* 去注册该设备，返回错误 */
            unregister_chrdev_region(ulDevno, 1);
            printk("[VCOM %d]APP_VCOM_Init malloc device buff fail. \n", ucIndex);
            kfree(pstVcomCtx->pstAppVcomDevEntity);
            pstVcomCtx->pstAppVcomDevEntity = VOS_NULL_PTR;
            return VOS_ERROR;
        }
        /* Modified by L60609 for add app vcom，2013-06-17,  End */

        init_waitqueue_head(&pstVcomDevp->Read_Wait);

        APP_VCOM_Setup(pstVcomDevp, ucIndex);

        /* 创建信号量 */
        sema_init(&pstVcomDevp->stMsgSendSem,1);

        sema_init(&pstVcomDevp->stWrtSem, 1);
    }

    return VOS_OK;
}


int APP_VCOM_Release(
    struct inode                       *inode,
    struct file                        *filp
)
{
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    VOS_UINT                            ulDevMajor;
    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    if (VOS_NULL_PTR == inode || VOS_NULL_PTR == filp)
    {
        return VOS_ERROR;
    }

    /* 获取主设备号 */
    ulDevMajor = imajor(inode);

    /* 根据主设备号得到设备在全局变量中的索引值 */
    ucIndex = APP_VCOM_GetIndexFromMajorDevId(ulDevMajor);

    if (ucIndex >= APP_VCOM_MAX_NUM)
    {
        vos_printf("APP_VCOM_Release ulIndex:%d is error. \n", ucIndex);
        return VOS_ERROR;
    }

    /* 获取VCOM全局变量 */
    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

    if (VOS_NULL_PTR == pstVcomCtx->pstAppVcomDevEntity)
    {
        vos_printf("[VCOM %d]APP_VCOM_Release VcomDevEntity is NULL. \n", ucIndex);
        return VOS_ERROR;
    }

    /* 将设备结构体指针赋值给文件私有数据指针 */
    filp->private_data = pstVcomCtx->pstAppVcomDevEntity;

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Release enter. \n", ucIndex);
    }

    if(NULL != pstVcomCtx->pEventFunc)
    {
        (VOS_VOID)pstVcomCtx->pEventFunc(APP_VCOM_EVT_RELEASE);
    }
#endif
    return VOS_OK;
}



int APP_VCOM_Open(
    struct inode                       *inode,
    struct file                        *filp
)
{
    VOS_UINT                            ulDevMajor;
    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    if (VOS_NULL_PTR == inode || VOS_NULL_PTR == filp)
    {
        return VOS_ERROR;
    }

    /* 获取主设备号 */
    ulDevMajor = imajor(inode);

    /* 根据主设备号得到设备在全局变量中的索引值 */
    ucIndex = APP_VCOM_GetIndexFromMajorDevId(ulDevMajor);

    if (ucIndex >= APP_VCOM_MAX_NUM)
    {
        vos_printf("APP_VCOM_Open ulIndex:%d is error. \n", ucIndex);
        return VOS_ERROR;
    }

    /* 获取VCOM全局变量 */
    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

    if (VOS_NULL_PTR == pstVcomCtx->pstAppVcomDevEntity)
    {
        vos_printf("[VCOM %d]APP_VCOM_Open VcomDevEntity is NULL. \n", ucIndex);
        return VOS_ERROR;
    }

    /* 将设备结构体指针赋值给文件私有数据指针 */
    filp->private_data = pstVcomCtx->pstAppVcomDevEntity;

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Open enter. \n", ucIndex);
    }

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
    if(pstVcomCtx->pEventFunc != NULL)
    {
        (VOS_VOID)pstVcomCtx->pEventFunc(APP_VCOM_EVT_OPEN);
    }
#endif
    return VOS_OK;
}


ssize_t APP_VCOM_Read(
    struct file                        *stFilp,
    char __user                        *buf,
    size_t                              count,
    loff_t                             *ppos
)
{
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDev;
    struct cdev                        *pstCdev;
    VOS_UINT                            ulDevMajor;
    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    /* 获得设备结构体指针 */
    pstVcomDev = stFilp->private_data;

    /* 获得设备主设备号 */
    pstCdev = &(pstVcomDev->stAppVcomDev);
    ulDevMajor = MAJOR(pstCdev->dev);

    /* 获得设备在全局变量中的索引值 */
    ucIndex = APP_VCOM_GetIndexFromMajorDevId(ulDevMajor);

    if (ucIndex >= APP_VCOM_MAX_NUM)
    {
        return APP_VCOM_ERROR;
    }

    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Read, read count:%d, current_len:%d.\n", ucIndex, count, pstVcomDev->current_len);
        vos_printf("[VCOM %d]APP_VCOM_Read, f_flags:%d.\n", ucIndex, stFilp->f_flags);
        vos_printf("[VCOM %d]APP_VCOM_Read, wait_event 111,flag:%d.\n", ucIndex,  pstVcomDev->ulReadWakeUpFlg);
    }

    if (stFilp->f_flags & O_NONBLOCK)
    {
        return APP_VCOM_ERROR;
    }

#if (FEATURE_ON == MBB_WPG_COMMON)
    if (VCOM_DISABLE_READ == (g_ulAppVcomDisableFlg & VCOM_DISABLE_READ))
    {
        if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
        {
            vos_printf("[VCOM %d]APP_VCOM_Read, read count:%d, current_len:%d. APP_VCOM_ERROR\n",
                        ucIndex, count, pstVcomDev->current_len);
        }
        return APP_VCOM_ERROR;
    }
#endif
    
#if (VOS_OS_VER == VOS_WIN32)

#else
    /*lint -e730 修改人:l60609;检视人:z60575;原因:两个线程会同时写该全局变量  */
    if (wait_event_interruptible(pstVcomDev->Read_Wait, (pstVcomDev->current_len != 0)))
    {
        return -ERESTARTSYS;
    }
    /*lint +e730 修改人:l60609;检视人:z60575;原因:两个线程会同时写该全局变量  */
#endif

    if (0 == pstVcomDev->current_len)
    {
        g_stAppVcomDebugInfo.ulReadLenErr[ucIndex]++;
    }

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Read, wait_event 222,flag:%d.\n", ucIndex, pstVcomDev->ulReadWakeUpFlg);
    }

    /* 获取信号量 */
    down(&pstVcomCtx->pstAppVcomDevEntity->stMsgSendSem);

    if (count > pstVcomDev->current_len)
    {
        count = pstVcomDev->current_len;
    }

    if (copy_to_user(buf, pstVcomDev->pucAppVcomMem, count))
    {
        vos_printf("[VCOM %d]APP_VCOM_Read, copy_to_user fail.\n", ucIndex);

        /* 释放信号量 */
        up(&pstVcomCtx->pstAppVcomDevEntity->stMsgSendSem);
        return APP_VCOM_ERROR;
    }

    if ((pstVcomDev->current_len - count) > 0)
    {
        /* FIFO数据前移 */
        memmove(pstVcomDev->pucAppVcomMem, (pstVcomDev->pucAppVcomMem + count), (pstVcomDev->current_len - count));

        if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
        {
            vos_printf("APP_VCOM_Read, FIFO move\n");
        }
    }

    /* 有效数据长度减小*/
    pstVcomDev->current_len -= count;

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Read, read %d bytes，current_len:%d\n", ucIndex, count, pstVcomDev->current_len);
    }

    /* 释放信号量 */
    up(&pstVcomCtx->pstAppVcomDevEntity->stMsgSendSem);

    return (ssize_t)count;
}


ssize_t APP_VCOM_Write(
    struct file                        *stFilp,
    const char __user                  *buf,
    size_t                              count,
    loff_t                             *ppos
)
{
    VOS_UINT8                          *pucDataBuf;
    VOS_INT                             iRst;
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDev;
    struct cdev                        *pstCdev;
    VOS_UINT                            ulDevMajor;
    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;

    /* 获得设备结构体指针 */
    pstVcomDev = stFilp->private_data;

    /* 获得设备主设备号 */
    pstCdev = &(pstVcomDev->stAppVcomDev);
    ulDevMajor = MAJOR(pstCdev->dev);

    /* 获得设备在全局变量中的索引值 */
    ucIndex = APP_VCOM_GetIndexFromMajorDevId(ulDevMajor);

    if(ucIndex >= APP_VCOM_MAX_NUM)
    {
        vos_printf("APP_VCOM_Write, ucIndex=%d fail\n", ucIndex);
        return APP_VCOM_ERROR;
    }
   
#if (FEATURE_ON == MBB_WPG_COMMON)
    if (VCOM_DISABLE_WRITE == (g_ulAppVcomDisableFlg & VCOM_DISABLE_WRITE))
    {
        if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
        {
            vos_printf("[VCOM %d]APP_VCOM_Write, write count:%d, current_len:%d. APP_VCOM_ERROR\n",
                        ucIndex, count, pstVcomDev->current_len);
        }
        return APP_VCOM_ERROR;
    }
#endif

    /* 获得全局变量地址 */
    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);


    /* 申请内存 */
    pucDataBuf = kmalloc(count, GFP_KERNEL);
    if (VOS_NULL_PTR == pucDataBuf )
    {
        vos_printf("[VCOM %d]APP_VCOM_Write, kmalloc fail\n", ucIndex);
        return APP_VCOM_ERROR;
    }

    /* buffer清零 */
    VOS_MemSet(pucDataBuf, 0x00, count);

    if (copy_from_user(pucDataBuf, buf, count))
    {
        vos_printf("[VCOM %d]APP_VCOM_Write, copy_from_user fail\n", ucIndex);
        kfree(pucDataBuf);
        return APP_VCOM_ERROR;
    }

#if (FEATURE_ON == FEATURE_VCOM_EXT)
    if ((APP_VCOM_DEV_INDEX_9 == ucIndex) || (APP_VCOM_DEV_INDEX_12 == ucIndex))
    {
        down(&pstVcomCtx->pstAppVcomDevEntity->stWrtSem);

        if (VOS_NULL_PTR != pstVcomCtx->pstAppVcomDevEntity->pucWrtBuffer)
        {
            vos_printf("[VCOM %d]APP_VCOM_Write: free buff.\n", ucIndex);
            kfree(pstVcomCtx->pstAppVcomDevEntity->pucWrtBuffer);
            pstVcomCtx->pstAppVcomDevEntity->pucWrtBuffer   = VOS_NULL_PTR;
            pstVcomCtx->pstAppVcomDevEntity->ulWrtBufferLen = 0;
        }

        if (VOS_NULL_PTR == pstVcomCtx->pSendUlAtFunc)
        {
            vos_printf("[VCOM %d]APP_VCOM_Write: save buff.\n", ucIndex);
            pstVcomCtx->pstAppVcomDevEntity->pucWrtBuffer   = pucDataBuf;
            pstVcomCtx->pstAppVcomDevEntity->ulWrtBufferLen = count;
            up(&pstVcomCtx->pstAppVcomDevEntity->stWrtSem);
            return (ssize_t)count;
        }

        up(&pstVcomCtx->pstAppVcomDevEntity->stWrtSem);
    }
#endif

    /* 调用回调函数处理buf中的AT码流*/
    if (VOS_NULL_PTR == pstVcomCtx->pSendUlAtFunc)
    {
        vos_printf("[VCOM %d]APP_VCOM_Write, pSendUlAtFunc is null\n", ucIndex);
        kfree(pucDataBuf);
        return APP_VCOM_ERROR;
    }

    iRst = pstVcomCtx->pSendUlAtFunc(ucIndex, pucDataBuf, count);
    if (VOS_OK != iRst)
    {
        g_stAppVcomDebugInfo.ulAtCallBackErr[ucIndex]++;
        vos_printf("[VCOM %d]APP_VCOM_Write, AT_RcvFromAppCom fail\n", ucIndex);
        kfree(pucDataBuf);

        return APP_VCOM_ERROR;
    }

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Write, AT_RcvFromAppCom Success\n", ucIndex);
    }

    /* 释放内存 */
    kfree(pucDataBuf);

    return (ssize_t)count;
}


unsigned int APP_VCOM_Poll(struct file *fp, struct poll_table_struct *wait)
{
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDev = VOS_NULL_PTR;
    unsigned int                        mask = 0;

    struct cdev                        *pstCdev;
    VOS_UINT                            ulDevMajor;
    VOS_UINT8                           ucIndex;

    pstVcomDev = fp->private_data;

    pstCdev = &(pstVcomDev->stAppVcomDev);
    ulDevMajor = MAJOR(pstCdev->dev);
    ucIndex = APP_VCOM_GetIndexFromMajorDevId(ulDevMajor);

    poll_wait(fp, &pstVcomDev->Read_Wait, wait);

    if (0 != pstVcomDev->current_len)
    {
        mask |= POLLIN | POLLRDNORM;
    }

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(ucIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Poll, mask = %d\n", ucIndex, mask);
    }

    return mask;
}


VOS_UINT32  APP_VCOM_Send (
    APP_VCOM_DEV_INDEX_UINT8            enDevIndex,
    VOS_UINT8                          *pData,
    VOS_UINT32                          uslength
)
{
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDev;
#if (FEATURE_ON == FEATURE_VCOM_EXT)
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;
#endif

    if (enDevIndex >= APP_VCOM_MAX_NUM)
    {
        g_stAppVcomDebugInfo.ulDevIndexErr++;
        vos_printf("APP_VCOM_Send, enDevIndex:%d is error\n", enDevIndex);
        return VOS_ERR;
    }

#if (FEATURE_ON == FEATURE_VCOM_EXT)
    pstVcomCtx = APP_VCOM_GetVcomCtxAddr(enDevIndex);
    if (VOS_NULL_PTR == pstVcomCtx)
    {
        vos_printf("[VCOM %d]APP_VCOM_Send, pstVcomCtx is null\n", enDevIndex);
        return VOS_ERR;
    }
#endif

    /* 获得设备实体指针 */
    pstVcomDev = APP_VCOM_GetAppVcomDevEntity(enDevIndex);

    if (VOS_NULL_PTR == pstVcomDev)
    {
        g_stAppVcomDebugInfo.ulVcomDevErr[enDevIndex]++;
        vos_printf("[VCOM %d]APP_VCOM_Send,pstVcomDev is null\n", enDevIndex);
        return VOS_ERR;
    }

#if (FEATURE_ON == FEATURE_VCOM_EXT)
    if ((APP_VCOM_DEV_INDEX_9 == enDevIndex) || (APP_VCOM_DEV_INDEX_12 == enDevIndex))
    {
        down(&pstVcomDev->stWrtSem);

        if (VOS_NULL_PTR != pstVcomDev->pucWrtBuffer)
        {
            if (VOS_NULL_PTR != pstVcomCtx->pSendUlAtFunc)
            {
                vos_printf("[VCOM %d]APP_VCOM_Send: handle buff.\n", enDevIndex);
                (VOS_VOID)pstVcomCtx->pSendUlAtFunc(enDevIndex,
                                        pstVcomDev->pucWrtBuffer,
                                        pstVcomDev->ulWrtBufferLen);

                kfree(pstVcomDev->pucWrtBuffer);
                pstVcomDev->pucWrtBuffer   = VOS_NULL_PTR;
                pstVcomDev->ulWrtBufferLen = 0;
            }
        }

        up(&pstVcomDev->stWrtSem);
    }
#endif

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(enDevIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Send, uslength:%d,current_len:%d\n", enDevIndex, uslength, pstVcomDev->current_len);
    }

    /* 获取信号量 */
    down(&pstVcomDev->stMsgSendSem);

    /* 队列满则直接返回 */
    if (g_astAppVcomCogfigTab[enDevIndex].ulAppVcomMemSize == pstVcomDev->current_len)
    {
        if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(enDevIndex)))
        {
            vos_printf("[VCOM %d]APP_VCOM_Send: VCOM MEM FULL\n", enDevIndex);
        }

        g_stAppVcomDebugInfo.ulMemFullErr[enDevIndex]++;

        up(&pstVcomDev->stMsgSendSem);
        return VOS_ERR;
    }

    /* 发送数据大于剩余Buffer大小 */
    if (uslength > (g_astAppVcomCogfigTab[enDevIndex].ulAppVcomMemSize - pstVcomDev->current_len))
    {
        if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(enDevIndex)))
        {
            vos_printf("[VCOM %d]APP_VCOM_Send: data more than Buffer\n", enDevIndex);
        }
        uslength = g_astAppVcomCogfigTab[enDevIndex].ulAppVcomMemSize - pstVcomDev->current_len;
    }

    /* 复制到BUFFER */
    memcpy(pstVcomDev->pucAppVcomMem + pstVcomDev->current_len, pData, uslength);
    pstVcomDev->current_len += uslength;

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(enDevIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Send, written %d byte(s), new len: %d\n",
            enDevIndex, uslength, pstVcomDev->current_len);
    }

    /* 释放信号量 */
    up(&pstVcomDev->stMsgSendSem);
#ifdef __PC_UT__

#else
    wake_up_interruptible(&pstVcomDev->Read_Wait);
#endif

    if (0 == pstVcomDev->current_len)
    {
        g_stAppVcomDebugInfo.ulSendLenErr[enDevIndex]++;
    }

    if (0 != (g_ulAppVcomDebugFlg & APP_VCOM_SET_BIT(enDevIndex)))
    {
        vos_printf("[VCOM %d]APP_VCOM_Send, wakeup.\n", enDevIndex);
    }

    return VOS_OK;
}

#if (FEATURE_ON == MBB_WPG_COMMON)

VOS_VOID APP_VCOM_Disable(VOS_UINT32 DisableSet)
{
    g_ulAppVcomDisableFlg = DisableSet;

    return;
}
#endif


VOS_UINT32 APP_VCOM_SetDebugFlag(VOS_UINT8 ucIndex)
{
    if (ucIndex >= APP_VCOM_MAX_NUM)
    {
         vos_printf("Error: The channel range is 0-%d\r\n", (APP_VCOM_MAX_NUM - 1));
         return g_ulAppVcomDebugFlg;
    }

    g_ulAppVcomDebugFlg |= APP_VCOM_SET_BIT(ucIndex);

    return g_ulAppVcomDebugFlg;
}


VOS_UINT32 APP_VCOM_DebugReset(VOS_VOID)
{
    g_ulAppVcomDebugFlg = 0;

    return g_ulAppVcomDebugFlg;
}


VOS_VOID APP_VCOM_ShowDebugInfo(VOS_VOID)
{
    int                                 i;
    vos_printf("App Vcom Debug Info:");
    vos_printf("Index Err: %d\r\n", g_stAppVcomDebugInfo.ulDevIndexErr);

    for (i = 0; i < APP_VCOM_MAX_NUM; i++)
    {
        vos_printf("\r\n");
        vos_printf("AppVcom[%d] Callback Function Return Err Num: %d\r\n", i, g_stAppVcomDebugInfo.ulAtCallBackErr[i]);
        vos_printf("AppVcom[%d] Mem Full Num:                     %d\r\n", i, g_stAppVcomDebugInfo.ulMemFullErr[i]);
        vos_printf("AppVcom[%d] Read Data Length = 0 Num:         %d\r\n", i, g_stAppVcomDebugInfo.ulReadLenErr[i]);
        vos_printf("AppVcom[%d] Send Data Length = 0 Num:         %d\r\n", i, g_stAppVcomDebugInfo.ulSendLenErr[i]);
        vos_printf("AppVcom[%d] Get App Vcom Dev Entity Err Num:  %d\r\n", i, g_stAppVcomDebugInfo.ulVcomDevErr[i]);
    }
}

#if (VOS_WIN32 == VOS_OS_VER)

VOS_VOID APP_VCOM_FreeMem(VOS_VOID)
{
    APP_VCOM_DEV_CTX_STRU              *pstVcomCtx;
    VOS_UINT8                           ucIndex;
    APP_VCOM_DEV_ENTITY_STRU           *pstVcomDevp;

    pstVcomCtx = VOS_NULL_PTR;
    pstVcomDevp  = VOS_NULL_PTR;

   for (ucIndex = 0; ucIndex < APP_VCOM_MAX_NUM; ucIndex++)
    {
        pstVcomCtx = APP_VCOM_GetVcomCtxAddr(ucIndex);

        pstVcomDevp = pstVcomCtx->pstAppVcomDevEntity;

        if (VOS_NULL_PTR != pstVcomDevp->pucAppVcomMem)
        {
            free(pstVcomDevp->pucAppVcomMem);
            pstVcomDevp->pucAppVcomMem = VOS_NULL_PTR;

        }

        if(VOS_NULL_PTR != pstVcomCtx->pstAppVcomDevEntity)
        {
            free(pstVcomCtx->pstAppVcomDevEntity);
            pstVcomCtx->pstAppVcomDevEntity = VOS_NULL_PTR;
        }
    }

   return;
}
#endif

#if (VOS_LINUX == VOS_OS_VER)
#if (FEATURE_ON == FEATURE_DELAY_MODEM_INIT)
module_init(APP_VCOM_Init);
#endif
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


