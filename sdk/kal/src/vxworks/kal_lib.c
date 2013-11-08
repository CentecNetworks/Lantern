#include "kal.h"

#define NS_INADDRSZ	4	/* IPv4 T_A */
#define NS_IN6ADDRSZ	16	/* IPv6 T_AAAA */
#define NS_INT16SZ	2	/* #/bytes of data in a u_int16_t */
#define SPRINTF(x) ((int32)kal_sprintf x)
#define X_INF		0x7ffffff0
#define X_STORE(c) { 	\
    if (bp < be)	\
	*bp = (c); 	\
    bp++; 		\
}


static const int8 *
inet_ntop4(const uint8 *src, int8 *dst, uint32 size)
{
	static const int8 fmt[] = "%u.%u.%u.%u";
	int8 tmp[sizeof "255.255.255.255"];

	if (SPRINTF((tmp, fmt, src[0], src[1], src[2], src[3])) > size) {
		return (NULL);
	}
	return kal_strcpy(dst, tmp);
}

static const int8 *
inet_ntop6(const uint8 *src, int8 *dst, uint32 size)
{
	int8 tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct { int base, len; } best, cur;
	uint32 words[NS_IN6ADDRSZ / NS_INT16SZ];
	int32 i;

    kal_memset(&best, 0, sizeof(best));
    kal_memset(&cur, 0, sizeof(cur));

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	kal_memset(words, '\0', sizeof words);
	for (i = 0; i < NS_IN6ADDRSZ; i += 2)
		words[i / 2] = (src[i] << 8) | src[i + 1];
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp)))
				return (NULL);
			tp += kal_strlen(tp);
			break;
		}
		tp += SPRINTF((tp, "%x", words[i]));
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) ==
	    (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((uint32)(tp - tmp) > size) {
		return (NULL);
	}
	return kal_strcpy(dst, tmp);
}

static int
inet_pton4(const int8 *src, uint8 *dst)
{
	int32 saw_digit, octets, ch;
	uint8 tmp[NS_INADDRSZ], *tp;
    uint32 new = 0;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {

		if (ch >= '0' && ch <= '9') {
			new = *tp * 10 + (ch - '0');

			if (saw_digit && *tp == 0)
				return (0);
			if (new > 255)
				return (0);
			*tp = new;
			if (! saw_digit) {
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);
	kal_memcpy(dst, tmp, NS_INADDRSZ);
	return (1);
}

static int
inet_pton6(const int8 *src, uint8 *dst)
{
	static const int8 xdigits[] = "0123456789abcdef";
	uint8 tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const int8 *curtok;
	int32 ch, saw_xdigit;
	uint32 val;

	tp = kal_memset(tmp, '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = kal_tolower (*src++)) != '\0') {
		const int8 *pch;

		pch = kal_strchr(xdigits, ch);
		if (pch != NULL) {
			val <<= 4;
			val |= ((uint32)pch - (uint32)xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			} else if (*src == '\0') {
				return (0);
			}
			if (tp + NS_INT16SZ > endp)
				return (0);
			*tp++ = (uint8) (val >> 8) & 0xff;
			*tp++ = (uint8) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		    inet_pton4(curtok, tp) > 0) {
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) {
		if (tp + NS_INT16SZ > endp)
			return (0);
		*tp++ = (uint8) (val >> 8) & 0xff;
		*tp++ = (uint8) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		if (tp == endp)
			return (0);
		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	kal_memcpy(dst, tmp, NS_IN6ADDRSZ);
	return (1);
}


const int8 *
kal_inet_ntop(int32 af, void *src, int8 *dst, uint32 size)
{
	switch (af) {
	case AF_INET:
		return (inet_ntop4(src, dst, size));
	case AF_INET6:
		return (inet_ntop6(src, dst, size));
	default:
		return (NULL);
	}
	/* NOTREACHED */
}

int32
kal_inet_pton(int32 af, const int8 *src, void *dst)
{
	switch (af) {
	case AF_INET:
		return (inet_pton4(src, dst));
	case AF_INET6:
		return (inet_pton6(src, dst));
	default:
		return (-1);
	}
	/* NOTREACHED */
}

int32
kal_strcasecmp (const int8 *s1, const int8 *s2)
{
  const uint8 *p1 = (const uint8 *) s1;
  const uint8 *p2 = (const uint8 *) s2;
  int32 ret;

  if (p1 == p2)
    return 0;

  while ((ret = tolower (*p1) - tolower (*p2++)) == 0)
    if (*p1++ == '\0')
      break;

  return ret;
}

int32
kal_strncasecmp(const int8 *s1, const int8 *s2, int32 n)
{
    uint8 c1, c2;

    if (n == 0)
    {
        return 0;
    }
    do
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (--n && c1 == c2 && c1 != 0);

    return c1 - c2;
}
#if 0
int8 *
kal_strdup(const int8 *s)
{
    int8 len = kal_strlen(s);
    int8 *rc = malloc(len + 1);
    if (rc != NULL)
    {
	    kal_strcpy(rc, s);
    }
    return rc;
}
#endif
void
kal_itoa(int8 *buf,     	/* Large enough result buffer	*/
	 uint32 num,		/* Number to convert		*/
	 int32 base,	     	/* Conversion base (2 to 16)	*/
	 int32 caps,	     	/* Capitalize letter digits	*/
	 int32 prec)		/* Precision (minimum digits)	*/
{
    int8		tmp[36], *s, *digits;

    digits = (caps ? "0123456789ABCDEF" : "0123456789abcdef");

    s = &tmp[sizeof (tmp) - 1];

    for (*s = 0; num || s == &tmp[sizeof (tmp) - 1]; num /= base, prec--)
	*--s = digits[num % base];

    while (prec-- > 0)
	*--s = '0';

    kal_strcpy(buf, s);
}

int32 kal_vsnprintf(int8 *buf, int32 bufsize, const int8 *fmt, va_list ap)
{
    int8 c, *bp, *be;
	int32 width = 0, ljust = 0, plus = 0, space = 0;
	int32 altform = 0, prec = 0, half = 0, base = 0;
	int32 tlong = 0, fillz = 0, plen, pad, prec_given = 0;
	int32 num = 0;
	int8 tmp[36], *p = tmp;

    bp = buf;
    be = (bufsize == X_INF) ? ((int8 *) 0) - 1 : &buf[bufsize - 1];

    while ((c = *fmt++) != 0)
    {
    	if (c != '%')
        {
    	    X_STORE(c);
    	    continue;
    	}

    	for (c = *fmt++; ; c = *fmt++)
    	    switch (c) {
    	    case 'h': half = 1;	 	break;
    	    case 'l': tlong = 1; 	break;
    	    case '-': ljust = 1; 	break;
    	    case '+': plus = 1; 	break;
    	    case ' ': space = 1; 	break;
    	    case '0': fillz = 1; 	break;
    	    case '#': altform = 1; 	break;
    	    case '*': width = -1;	break;	/* Mark as need-to-fetch */
    	    case '.':
    		if ((c = *fmt++) == '*')
    		    prec = -1;			/* Mark as need-to-fetch */
    		else {
    		    for (prec = 0; c >= '0' && c <= '9'; c = *fmt++)
    			prec = prec * 10 + (c - '0');
    		    fmt--;
    		}
    		prec_given = 1;
    		break;
    	    default:
    		if (c >= '1' && c <= '9') {
    		    for (width = 0; c >= '0' && c <= '9'; c = *fmt++)
    			width = width * 10 + (c - '0');
    		    fmt--;
    		} else
    		    goto break_for;
    		break;
    	    }
        break_for:

    	if (width == -1)
    	    width = va_arg(ap,int);
    	if (prec == -1)
    	    prec = va_arg(ap,int);

    	if (c == 0)
    	    break;

    	switch (c) {
    	case 'd':
    	case 'i':
    	    num = tlong ? va_arg(ap, long) : va_arg(ap, int);
    	    if (half)
    		num = (int) (short) num;
    	    /* For zero-fill, the sign must be to the left of the zeroes */
    	    if (fillz && (num < 0 || plus || space)) {
    		X_STORE(num < 0 ? '-' : space ? ' ' : '+');
    		if (width > 0)
    		    width--;
    		if (num < 0)
    		    num = -num;
    	    }
    	    if (! fillz) {
    		if (num < 0) {
    		    *p++ = '-';
    		    num = -num;
    		} else if (plus)
    		    *p++ = '+';
    		else if (space)
    		    *p++ = ' ';
    	    }
    	    base = 10;
    	    break;
    	case 'u':
    	    num = tlong ? va_arg(ap, long) : va_arg(ap, int);
    	    if (half)
    		num = (int) (short) num;
    	    base = 10;
    	    break;
    	case 'p':
    	    altform = 0;
    	    /* Fall through */
    	case 'x':
    	case 'X':
    	    num = tlong ? va_arg(ap, long) : va_arg(ap, int);
    	    if (half)
    		num = (int) (unsigned short) num;
    	    if (altform) {
    		prec += 2;
    		*p++ = '0';
    		*p++ = c;
    	    }
    	    base = 16;
    	    break;
    	case 'o':
    	case 'O':
    	    num = tlong ? va_arg(ap, long) : va_arg(ap, int);
    	    if (half)
    		num = (int) (unsigned short) num;
    	    if (altform) {
    		prec++;
    		*p++ = '0';
    	    }
    	    base = 8;
    	    break;
    	case 's':
    	    p = va_arg(ap,int8 *);
    	    if (prec == 0)
    		prec = X_INF;
    	    break;
    	case 'c':
    	    p[0] = va_arg(ap,int);
    	    p[1] = 0;
    	    prec = 1;
    	    break;
    	case 'n':
    	    *va_arg(ap,int *) = bp - buf;
    	    p[0] = 0;
    	    break;
    	case '%':
    	    p[0] = '%';
    	    p[1] = 0;
    	    prec = 1;
    	    break;
    	default:
    	    X_STORE(c);
    	    continue;
    	}

    	if (base != 0) {
    	    kal_itoa(p, (unsigned int) num, base, (c == 'X'), prec);
    	    if (prec)
    		fillz = 0;
    	    p = tmp;
    	    prec = X_INF;
    	}

    	if ((plen = kal_strlen(p)) > prec)
    	    plen = prec;

    	if (width < plen)
    	    width = plen;

    	pad = width - plen;

    	while (! ljust && pad-- > 0)
    	    X_STORE(fillz ? '0' : ' ');
    	for (; plen-- > 0 && width-- > 0; p++)
    	    X_STORE(*p);
    	while (pad-- > 0)
    	    X_STORE(' ');
    }

    if (bp < be)
	*bp = 0;
    else
	*be = 0;

    return (bp - buf);
}

int32 kal_vsprintf(int8 *buf, const int8 *fmt, va_list ap)
{
    return kal_vsnprintf(buf, (int32)X_INF, fmt, ap);
}

int32 kal_snprintf(int8 *buf, int32 bufsize, const int8 *fmt, ...)
{
    va_list	ap;
    int32 r;

    va_start(ap,fmt);
    r = kal_vsnprintf(buf, bufsize, fmt, ap);
    va_end(ap);

    return r;
}

