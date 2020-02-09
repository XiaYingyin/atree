/*
 * fktup.h
 *	  POSTGRES index tuple definitions.
 *
 *
 * Portions Copyright (c) 2019, Regents of the Renmin University of China
 *
 * nitup.h
 *
 */

#ifndef NITUP_H
#define NITUP_H
typedef struct FITingTreeLeafData
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

} FITingTreeLeafData;				/* MORE DATA FOLLOWS AT END OF STRUCT */

typedef FITingTreeLeafData *FKeyTuple;

/*
 * t_info manipulation macros
 */
#define KEY_SIZE_MASK 0x1FFF
#define KEY_AM_RESERVED_BIT 0x2000	/* reserved for index-AM specific
										 * usage */
#define KEY_VAR_MASK	0x4000
#define KEY_NULL_MASK 0x8000

#define FKeyTupleSize(ktup)		((Size) ((ktup)->t_info & KEY_SIZE_MASK))
#define FKeyTupleHasNulls(ktup)	((((FKeyTuple) (ktup))->t_info & KEY_NULL_MASK))
#define FKeyTupleHasVarwidths(ktup) ((((FKeyTuple) (ktup))->t_info & KEY_VAR_MASK))
/*
 * Takes an infomask as argument (primarily because this needs to be usable
 * at index_form_tuple time so enough space is allocated).
 */
#define KeyInfoFindDataOffset(t_info) \
( \
	(!((t_info) & INDEX_NULL_MASK)) ? \
	( \
		(Size)MAXALIGN(sizeof(FITingTreeLeafData)) \
	) \
	: \
	( \
		(Size)MAXALIGN(sizeof(FITingTreeLeafData) + sizeof(IndexAttributeBitMapData)) \
	) \
)

/* ----------------
 *		index_getattr
 *
 *		This gets called many times, so we macro the cacheable and NULL
 *		lookups, and call nocache_index_getattr() for the rest.
 *
 * ----------------
 */
#define index_getattr(tup, attnum, tupleDesc, isnull) \
( \
	AssertMacro(PointerIsValid(isnull) && (attnum) > 0), \
	*(isnull) = false, \
	!IndexTupleHasNulls(tup) ? \
	( \
		TupleDescAttr((tupleDesc), (attnum)-1)->attcacheoff >= 0 ? \
		( \
			fetchatt(TupleDescAttr((tupleDesc), (attnum)-1), \
			(char *) (tup) + IndexInfoFindDataOffset((tup)->t_info) \
			+ TupleDescAttr((tupleDesc), (attnum)-1)->attcacheoff) \
		) \
		: \
			nocache_index_getattr((tup), (attnum), (tupleDesc)) \
	) \
	: \
	( \
		(att_isnull((attnum)-1, (char *)(tup) + sizeof(FITingTreeLeafData))) ? \
		( \
			*(isnull) = true, \
			(Datum)NULL \
		) \
		: \
		( \
			nocache_index_getattr((tup), (attnum), (tupleDesc)) \
		) \
	) \
)

/*
 * MaxIndexTuplesPerPage is an upper bound on the number of tuples that can
 * fit on one index page.  An index tuple must have either data or a null
 * bitmap, so we can safely assume it's at least 1 byte bigger than a bare
 * FITingTreeLeafData struct.  We arrive at the divisor because each tuple
 * must be maxaligned, and it must have an associated item pointer.
 *
 * To be index-type-independent, this does not account for any special space
 * on the page, and is thus conservative.
 *
 * Note: in btree non-leaf pages, the first tuple has no key (it's implicitly
 * minus infinity), thus breaking the "at least 1 byte bigger" assumption.
 * On such a page, N tuples could take one MAXALIGN quantum less space than
 * estimated here, seemingly allowing one more tuple than estimated here.
 * But such a page always has at least MAXALIGN special space, so we're safe.
 */
#define MaxIndexTuplesPerPage	\
	((int) ((BLCKSZ - SizeOfPageHeaderData) / \
			(MAXALIGN(sizeof(FITingTreeLeafData) + 1) + sizeof(ItemIdData))))

#endif
