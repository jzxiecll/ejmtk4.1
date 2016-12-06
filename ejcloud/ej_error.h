/*! \file wmerrno.h
 *  \brief Error Management
 *
 *  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 *
 */
#ifndef __EJ_ERROR_H__
#define __EJ_ERROR_H__



enum ej_errno {
	/* First Generic Error codes */
	EJ_SUCCESS = 0,
	EJ_FAIL,     /* 1 */
	EJ_E_PERM,   /* 2: Operation not permitted */
	EJ_E_NOENT,  /* 3: No such file or directory */
	EJ_E_SRCH,   /* 4: No such process */
	EJ_E_INTR,   /* 5: Interrupted system call */
	EJ_E_IO,     /* 6: I/O error */
	EJ_E_NXIO,   /* 7: No such device or address */
	EJ_E_2BIG,   /* 8: Argument list too long */
	EJ_E_NOEXEC, /* 9: Exec format error */
	EJ_E_BADF,   /* 10: Bad file number */
	EJ_E_CHILD,  /* 11: No child processes */
	EJ_E_AGAIN,  /* 12: Try again */
	EJ_E_NOMEM,  /* 13: Out of memory */
	EJ_E_ACCES,  /* 14: Permission denied */
	EJ_E_FAULT,  /* 15: Bad address */
	EJ_E_NOTBLK, /* 16: Block device required */
	EJ_E_BUSY,   /* 17: Device or resource busy */
	EJ_E_EXIST,  /* 18: File exists */
	EJ_E_XDEV,   /* 19: Cross-device link */
	EJ_E_NODEV,  /* 20: No such device */
	EJ_E_NOTDIR, /* 21: Not a directory */
	EJ_E_ISDIR,  /* 22: Is a directory */
	EJ_E_INVAL,  /* 23: Invalid argument */
	EJ_E_NFILE,  /* 24: File table overflow */
	EJ_E_MFILE,  /* 25: Too many open files */
	EJ_E_NOTTY,  /* 26: Not a typewriter */
	EJ_E_TXTBSY, /* 27: Text file busy */
	EJ_E_FBIG,   /* 28: File too large */
	EJ_E_NOSPC,  /* 29: No space left on device */
	EJ_E_SPIPE,  /* 30: Illegal seek */
	EJ_E_ROFS,   /* 31: Read-only file system */
	EJ_E_MLINK,  /* 32: Too many links */
	EJ_E_PIPE,   /* 33: Broken pipe */
	EJ_E_DOM,    /* 34: Math argument out of domain of func */
	EJ_E_RANGE,  /* 35: Math result not representable */

	/* WMSDK generic error codes */
	EJ_E_CRC,    /* 36: Error in CRC check */
	EJ_E_UNINIT,  /* 37: Module is not yet initialized */
	EJ_E_TIMEOUT, /* 38: Timeout occurred during operation */
};

#endif /* ! WM_ERRNO_H */

