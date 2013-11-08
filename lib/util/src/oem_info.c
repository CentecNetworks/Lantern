/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dirent.h>
#include "sal.h"
#ifdef _CTC_OF_
#include "genlog.h"
#else
#include "lib.h"
#include "snprintf.h"
#endif
#include "oem_info.h"

oem_info_t g_st_oem_info;
int g_oem_init = 0;

void
oem_info_init(void)
{
    FILE *fp;
    char buf[OEM_INFO_BUFSIZ];

    sal_memset(&g_st_oem_info, 0, sizeof(oem_info_t));
    sal_strncpy(g_st_oem_info.company_name, "Unknown", OEM_INFO_BUFSIZ);
    sal_strncpy(g_st_oem_info.package_name, "Unknown", OEM_INFO_BUFSIZ);
    sal_strncpy(g_st_oem_info.product_name, "Unknown", OEM_INFO_BUFSIZ);
    sal_strncpy(g_st_oem_info.hardware_type, "Unknown", OEM_INFO_BUFSIZ);
    sal_strncpy(g_st_oem_info.snmp_enterprise_oid, "27975", OEM_INFO_BUFSIZ);

    fp = sal_fopen(OEM_INFO_FILE, "r");
    if(NULL == fp)
    {
        log_sys(M_MOD_LIB, E_ERROR, "OEM information file is not exist.");
        return;
    }

    while(sal_fgets(buf, OEM_INFO_BUFSIZ, fp) != NULL)
    {
        buf[sal_strlen(buf)-1]= '\0';
        if(!sal_strncmp(buf, OEM_COMPANY_FULL_NAME, sal_strlen(OEM_COMPANY_FULL_NAME)))
        {
            sal_memcpy(g_st_oem_info.company_name, buf+sizeof(OEM_COMPANY_FULL_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_COMPANY_FULL_NAME));
        }
        else if(!sal_strncmp(buf, OEM_PACKAGE_NAME, sal_strlen(OEM_PACKAGE_NAME)))
        {
            sal_memcpy(g_st_oem_info.package_name, buf+sizeof(OEM_PACKAGE_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_PACKAGE_NAME));
        }
        else if(!sal_strncmp(buf, OEM_PRODUCT_NAME, sal_strlen(OEM_PRODUCT_NAME)))
        {
            sal_memcpy(g_st_oem_info.product_name, buf+sizeof(OEM_PRODUCT_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_PRODUCT_NAME));
        }
        else if(!sal_strncmp(buf, OEM_HARDWARE_TYPE, sal_strlen(OEM_HARDWARE_TYPE)))
        {
            sal_memcpy(g_st_oem_info.hardware_type, buf+sizeof(OEM_HARDWARE_TYPE),
                    OEM_INFO_BUFSIZ-sizeof(OEM_HARDWARE_TYPE));
        }
        else if(!sal_strncmp(buf, OEM_SNMP_ENTERPRISE_OID, sal_strlen(OEM_SNMP_ENTERPRISE_OID)))
        {
            sal_memcpy(g_st_oem_info.snmp_enterprise_oid, buf+sizeof(OEM_SNMP_ENTERPRISE_OID),
                    OEM_INFO_BUFSIZ-sizeof(OEM_SNMP_ENTERPRISE_OID));
        }
        else if(!sal_strncmp(buf, OEM_CHASSIS_TYPE, sal_strlen(OEM_CHASSIS_TYPE)))
        {
            sal_memcpy(g_st_oem_info.chassis_type, buf+sizeof(OEM_CHASSIS_TYPE),
                    OEM_INFO_BUFSIZ-sizeof(OEM_CHASSIS_TYPE));
        }
        else if(!sal_strncmp(buf, OEM_4SFP_CARD_NAME, sal_strlen(OEM_4SFP_CARD_NAME)))
        {
            sal_memcpy(g_st_oem_info.four_sfp_name, buf+sizeof(OEM_4SFP_CARD_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_4SFP_CARD_NAME));
        }
        else if(!sal_strncmp(buf, OEM_2SFPP_CARD_NAME, sal_strlen(OEM_2SFPP_CARD_NAME)))
        {
            sal_memcpy(g_st_oem_info.two_sfpp_name, buf+sizeof(OEM_2SFPP_CARD_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_2SFPP_CARD_NAME));
        }
        else if(!sal_strncmp(buf, OEM_4SFPP_CARD_NAME, sal_strlen(OEM_4SFPP_CARD_NAME)))
        {
            sal_memcpy(g_st_oem_info.four_sfpp_name, buf+sizeof(OEM_4SFPP_CARD_NAME),
                    OEM_INFO_BUFSIZ-sizeof(OEM_4SFPP_CARD_NAME));
        }
    }

    sal_fclose(fp);
    return;
}


char* oem_info_get_info_by_type(int type, char *buf)
{
    if (g_oem_init == 0)
    {
        oem_info_init();
        g_oem_init = 1;
    }

    switch (type)
    {
        case OEM_TYPE_COMPANY_NAME:
            if (buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.company_name);
            return g_st_oem_info.company_name;

        case OEM_TYPE_PACKAGE_NAME:
            if (buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.package_name);
            return g_st_oem_info.package_name;

        case OEM_TYPE_PRODUCT_NAME:
            if (buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.product_name);
            return g_st_oem_info.product_name;

        case OEM_TYPE_HARDWARE_TYPE:
            if (buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.hardware_type);
            return g_st_oem_info.hardware_type;

        case OEM_TYPE_SNMP_ENTERPRISE_OID:
            if (buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.snmp_enterprise_oid);
            return g_st_oem_info.snmp_enterprise_oid;
        case OEM_TYPE_4SFP_CARD_NAME:
            if(buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.four_sfp_name);
            if((g_st_oem_info.four_sfp_name[0]!='\0')
                &&(sal_strncmp(g_st_oem_info.four_sfp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
                return g_st_oem_info.four_sfp_name;
            else
                return NULL;
        case OEM_TYPE_2SFPP_CARD_NAME:
            if(buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.two_sfpp_name);
            if((g_st_oem_info.two_sfpp_name[0]!='\0')
                &&(sal_strncmp(g_st_oem_info.two_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
                return g_st_oem_info.two_sfpp_name;
            else
                return NULL;
        case OEM_TYPE_4SFPP_CARD_NAME:
            if(buf)
                sal_snprintf(buf, OEM_INFO_BUFSIZ, g_st_oem_info.four_sfpp_name);
            if((g_st_oem_info.four_sfpp_name[0]!='\0')
                &&(sal_strncmp(g_st_oem_info.four_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
                return g_st_oem_info.four_sfpp_name;
            else
                return NULL;
        default:
            break;
    }

    return NULL;
}

char* oem_info_get_company_name(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_COMPANY_NAME, buf);
}

char* oem_info_get_package_name(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_PACKAGE_NAME, buf);
}

char* oem_info_get_product_name(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_PRODUCT_NAME, buf);
}

char* oem_info_get_hardware_type(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_HARDWARE_TYPE, buf);
}

char* oem_info_get_chassis_type(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_CHASSIS_TYPE, buf);
}

char* oem_info_get_snmp_enterprise_oid(char *buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_SNMP_ENTERPRISE_OID, buf);
}

int oem_info_get_single_snmp_enterprise_oid()
{
    char enterprise_oid[OEM_INFO_BUFSIZ];

    oem_info_get_snmp_enterprise_oid(enterprise_oid);
    return sal_atoi(enterprise_oid);
}

int
oem_info_get_oem_vendor_type()
{
    char package[OEM_INFO_BUFSIZ];
    oem_info_get_package_name(package);
    if (sal_strstr (package, "Dummy"))
    {
        return OEM_VENDOR_TYPE_DUMMY;
    }
    else
        return OEM_VENDOR_TYPE_CENTEC;
}

char* oem_info_get_4sfp_card_name(char* buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_4SFP_CARD_NAME, buf);
}
char* oem_info_get_2sfpp_card_name(char* buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_2SFPP_CARD_NAME, buf);
}
char* oem_info_get_4sfpp_card_name(char* buf)
{
    return oem_info_get_info_by_type(OEM_TYPE_4SFPP_CARD_NAME, buf);
}


int oem_create_oem_info_file(oem_info_t *oem_info)
{
    FILE *fp;

    fp = sal_fopen(OEM_INFO_FILE, "w+");
    if(NULL == fp)
    {
        log_sys(M_MOD_LIB, E_ERROR, "Open oem information file error.");
        return -1;
    }

    if (oem_info)
    {
    	if(0 != strcmp(oem_info->package_name, "CentecOS"))
		{
	        sal_fprintf(fp, "%s %s\n", OEM_COMPANY_FULL_NAME, oem_info->company_name);
	        sal_fprintf(fp, "%s %s\n", OEM_PACKAGE_NAME, oem_info->package_name);
	        sal_fprintf(fp, "%s %s\n", OEM_PRODUCT_NAME, oem_info->product_name);
	    }
	    else
	    {
            sal_fprintf(fp, "%s %s\n", OEM_COMPANY_FULL_NAME, OEM_COMPANY_REAL_NAME);
            sal_fprintf(fp, "%s %s\n", OEM_PACKAGE_NAME, OEM_PACKAGE_REAL_NAME);
            sal_fprintf(fp, "%s %s\n", OEM_PRODUCT_NAME, OEM_PRODUCT_REAL_NAME);
	    }

		sal_fprintf(fp, "%s %s\n", OEM_HARDWARE_TYPE, oem_info->hardware_type);
	    sal_fprintf(fp, "%s %s\n", OEM_SNMP_ENTERPRISE_OID, oem_info->snmp_enterprise_oid);
	    sal_fprintf(fp, "%s %s\n", OEM_CHASSIS_TYPE, oem_info->chassis_type);

	    if((oem_info->four_sfp_name[0]!='\0')
	        &&(sal_strncmp(oem_info->four_sfp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
	    {
	        sal_fprintf(fp, "%s %s\n", OEM_4SFP_CARD_NAME, oem_info->four_sfp_name);
	    }
	    if((oem_info->two_sfpp_name[0]!='\0')
	        &&(sal_strncmp(oem_info->two_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
	    {
	        sal_fprintf(fp, "%s %s\n", OEM_2SFPP_CARD_NAME, oem_info->two_sfpp_name);
	    }
	    if((oem_info->four_sfpp_name[0]!='\0')
	        &&(sal_strncmp(oem_info->four_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0))
	    {
	        sal_fprintf(fp, "%s %s\n", OEM_4SFPP_CARD_NAME, oem_info->four_sfpp_name);
	    }
	}
	else
	{
        sal_fprintf(fp, "%s %s\n", OEM_COMPANY_FULL_NAME, "Unknown");
        sal_fprintf(fp, "%s %s\n", OEM_PACKAGE_NAME, "Unknown");
        sal_fprintf(fp, "%s %s\n", OEM_PRODUCT_NAME, "Unknown");
        sal_fprintf(fp, "%s %s\n", OEM_HARDWARE_TYPE, "Unknown");
        sal_fprintf(fp, "%s %s\n", OEM_SNMP_ENTERPRISE_OID, "27975");
        sal_fprintf(fp, "%s %s\n", OEM_CHASSIS_TYPE, "Unknown");
	}

    sal_fclose(fp);
    return 0;
}

int oem_encode_oem_info(char buffer[], oem_info_t *oem_info)
{
    int len = 0;
    int unit_len = 0;

    if (!buffer || !oem_info)
    {
        return -1;
    }

    buffer[len++] = OEM_TYPE_COMPANY_NAME;
    unit_len = sal_strlen (oem_info->company_name);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->company_name, unit_len);
    len += unit_len;

    buffer[len++] = OEM_TYPE_PACKAGE_NAME;
    unit_len = sal_strlen (oem_info->package_name);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->package_name, unit_len);
    len += unit_len;

    buffer[len++] = OEM_TYPE_PRODUCT_NAME;
    unit_len = sal_strlen (oem_info->product_name);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->product_name, unit_len);
    len += unit_len;

    buffer[len++] = OEM_TYPE_HARDWARE_TYPE;
    unit_len = sal_strlen (oem_info->hardware_type);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->hardware_type, unit_len);
    len += unit_len;

    buffer[len++] = OEM_TYPE_SNMP_ENTERPRISE_OID;
    unit_len = sal_strlen (oem_info->snmp_enterprise_oid);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->snmp_enterprise_oid, unit_len);
    len += unit_len;

    buffer[len++] = OEM_TYPE_CHASSIS_TYPE;
    unit_len = sal_strlen (oem_info->chassis_type);
    buffer[len++] = (char)unit_len;
    sal_memcpy(&buffer[len], oem_info->chassis_type, unit_len);
    len += unit_len;

    /*destroy old oem external card name info.*/
    buffer[len] = '\0';
    if(sal_strncmp(oem_info->four_sfp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0)
    {
        buffer[len++] = OEM_TYPE_4SFP_CARD_NAME;
        unit_len = sal_strlen (oem_info->four_sfp_name);
        buffer[len++] = (char)unit_len;
        sal_memcpy(&buffer[len], oem_info->four_sfp_name, unit_len);
        len += unit_len;
    }

    if(sal_strncmp(oem_info->two_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0)
    {
        buffer[len++] = OEM_TYPE_2SFPP_CARD_NAME;
        unit_len = sal_strlen (oem_info->two_sfpp_name);
        buffer[len++] = (char)unit_len;
        sal_memcpy(&buffer[len], oem_info->two_sfpp_name, unit_len);
        len += unit_len;
    }

    if(sal_strncmp(oem_info->four_sfpp_name, OEM_DEFAULT_NAME, sizeof(OEM_DEFAULT_NAME)) != 0)
    {
        buffer[len++] = OEM_TYPE_4SFPP_CARD_NAME;
        unit_len = sal_strlen (oem_info->four_sfpp_name);
        buffer[len++] = (char)unit_len;
        sal_memcpy(&buffer[len], oem_info->four_sfpp_name, unit_len);
        len += unit_len;
    }
    return 0;
}

int oem_decode_oem_info(char buffer[], oem_info_t *oem_info)
{
    int len = 0;
    int unit_len = 0;
    char year;
    int old_year = 0;
    int new_year = 0;

    /*get company_name*/
    if (buffer[len] != OEM_TYPE_COMPANY_NAME)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    year = buffer[len+6];
    new_year = sal_atoi(OEM_CENTEC_YEAR);
    if ('-' == year && unit_len > 9)
    {
        sal_memcpy(&oem_info->company_name, &buffer[len+2], 9);
        oem_info->company_name[9] = '\0';
        old_year = sal_atoi(&oem_info->company_name[5]);
        if (old_year < new_year)
            sal_memcpy(&oem_info->company_name[5], OEM_CENTEC_YEAR, 4);
        sal_memcpy(&oem_info->company_name[9], &buffer[len+11], unit_len - 9);
    }
    else if (' ' == year && unit_len > 4)
    {
        sal_memcpy(oem_info->company_name, &buffer[len+2], unit_len);
        oem_info->company_name[4] = '\0';
        old_year = sal_atoi(oem_info->company_name);
        if (old_year < new_year)
        {
            sal_memcpy(&oem_info->company_name, OEM_CENTEC_YEAR, 4);
        }
        oem_info->company_name[4] = ' ';
    }
    else
    {
        sal_memcpy(oem_info->company_name, &buffer[len+2], unit_len);
    }
    oem_info->company_name[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get package_name*/
    if (buffer[len] != OEM_TYPE_PACKAGE_NAME)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    sal_memcpy(oem_info->package_name, &buffer[len+2], unit_len);
    oem_info->package_name[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get product_name*/
    if (buffer[len] != OEM_TYPE_PRODUCT_NAME)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    sal_memcpy(oem_info->product_name, &buffer[len+2], unit_len);
    oem_info->product_name[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get hardware_type*/
    if (buffer[len] != OEM_TYPE_HARDWARE_TYPE)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    sal_memcpy(oem_info->hardware_type, &buffer[len+2], unit_len);
    oem_info->hardware_type[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get snmp_enterprise_oid*/
    if (buffer[len] != OEM_TYPE_SNMP_ENTERPRISE_OID)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    sal_memcpy(oem_info->snmp_enterprise_oid, &buffer[len+2], unit_len);
    oem_info->snmp_enterprise_oid[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get chassis_type*/
    if (buffer[len] != OEM_TYPE_CHASSIS_TYPE)
    {
        return -1;
    }

    unit_len =buffer[len+1];
    sal_memcpy(oem_info->chassis_type, &buffer[len+2], unit_len);
    oem_info->chassis_type[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get 4sfp card name*/
    if(buffer[len] != OEM_TYPE_4SFP_CARD_NAME)
    {
        oem_info->four_sfp_name[0] = '\0';
        oem_info->two_sfpp_name[0] = '\0';
        oem_info->four_sfpp_name[0] = '\0';
        return 0;
    }
    unit_len =buffer[len+1];
    sal_memcpy(oem_info->four_sfp_name, &buffer[len+2], unit_len);
    oem_info->four_sfp_name[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get 2sfpp card name*/
    if(buffer[len] != OEM_TYPE_2SFPP_CARD_NAME)
    {
        oem_info->four_sfp_name[0] = '\0';
        oem_info->two_sfpp_name[0] = '\0';
        oem_info->four_sfpp_name[0] = '\0';
        return 0;
    }
    unit_len =buffer[len+1];
    sal_memcpy(oem_info->two_sfpp_name, &buffer[len+2], unit_len);
    oem_info->two_sfpp_name[unit_len] = '\0';
    len = len + unit_len + 2;

    /*get 4sfpp card name*/
    if(buffer[len] != OEM_TYPE_4SFPP_CARD_NAME)
    {
        oem_info->four_sfp_name[0] = '\0';
        oem_info->two_sfpp_name[0] = '\0';
        oem_info->four_sfpp_name[0] = '\0';
        return 0;
    }
    unit_len =buffer[len+1];
    sal_memcpy(oem_info->four_sfpp_name, &buffer[len+2], unit_len);
    oem_info->four_sfpp_name[unit_len] = '\0';
    len = len + unit_len + 2;
    return 0;
}
