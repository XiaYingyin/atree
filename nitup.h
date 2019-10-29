/*-------------------------------------------------------------------------
 *
 * nitup.h
 *	  POSTGRES index tuple definitions.
 *
 *
 * Portions Copyright (c) 2019, Regents of the Renmin University of China
 *
 * nitup.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef NITUP_H
#define NITUP_H
typedef struct NIndexTupleData
{
	float slope;		/* the slope of linear function */

	/* ---------------
	 * t_info is laid out in the following fashion:
	 *
	 * 15th (high) bit: has nulls
	 * 14th bit: has var-width attributes
	 * 13th bit: AM-defined meaning
	 * 12-0 bit: size of tuple
	 * ---------------
	 */

	unsigned short t_info;		/* various info about tuple */

} IndexTupleData;				/* MORE DATA FOLLOWS AT END OF STRUCT */
#endif
