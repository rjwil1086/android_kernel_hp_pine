#ifndef __FT_ELAN_TP_H__
#define __FT_ELAN_TP_H__

typedef enum {
	TP_INIT = 0,
	TP_FT_OK,
	TP_FT_FAIL,
	TP_ELAN_OK,
	TP_ELAN_FAIL,
	TP_ELAN_TOPTOUCH_OK,
	TP_ELAN_TOPTOUCH_FAIL,
	TP_EXIT
} tp_status_t;

// extern tp_status_t g_tp_status;

#endif