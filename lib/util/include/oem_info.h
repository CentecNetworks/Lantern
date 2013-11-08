#ifndef _OEM_INFO_H
#define _OEM_INFO_H


#define OEM_INFO_FILE       "/tmp/oem_info"

#define OEM_COMPANY_FULL_NAME          "company_full_name"
#define OEM_PACKAGE_NAME          "package_name"
#define OEM_PRODUCT_NAME          "product_name"
#define OEM_HARDWARE_TYPE          "hardware_type"
#define OEM_SNMP_ENTERPRISE_OID          "snmp_enterprise_oid"
#define OEM_CHASSIS_TYPE                 "chassis_type"
#define OEM_4SFP_CARD_NAME               "4sfp_name"
#define OEM_2SFPP_CARD_NAME               "2sfpp_name"
#define OEM_4SFPP_CARD_NAME               "4sfpp_name"


#define OEM_DEFAULT_NAME "default"
#define OEM_CENTEC_YEAR            "2013"
#define OEM_INFO_BUFSIZ 256

#define OEM_COMPANY_REAL_NAME   "2004-2013 Centec Networks Inc"
#define OEM_PACKAGE_REAL_NAME   "CentecOS"
#define OEM_PRODUCT_REAL_NAME   "V330"

typedef enum
{
    OEM_VENDOR_TYPE_CENTEC = 1,
    OEM_VENDOR_TYPE_DUMMY,
    OEM_VENDOR_TYPE_MAX
} oem_verdor_type_e;

typedef enum
{
    OEM_TYPE_COMPANY_NAME = 1,
    OEM_TYPE_PACKAGE_NAME,
    OEM_TYPE_PRODUCT_NAME,
    OEM_TYPE_HARDWARE_TYPE,
    OEM_TYPE_SNMP_ENTERPRISE_OID,
    OEM_TYPE_CHASSIS_TYPE,
    OEM_TYPE_4SFP_CARD_NAME,
    OEM_TYPE_2SFPP_CARD_NAME,
    OEM_TYPE_4SFPP_CARD_NAME,
    OEM_TYPE_MAX
} oem_info_type_e;

typedef struct oem_info_s
{
    char company_name[OEM_INFO_BUFSIZ];
    char package_name[OEM_INFO_BUFSIZ];
    char product_name[OEM_INFO_BUFSIZ];
    char hardware_type[OEM_INFO_BUFSIZ];
    char snmp_enterprise_oid[OEM_INFO_BUFSIZ];
    char chassis_type[OEM_INFO_BUFSIZ];
    char four_sfp_name[OEM_INFO_BUFSIZ]; /*4SFP card name*/
    char two_sfpp_name[OEM_INFO_BUFSIZ];/*2SFP+ card name*/
    char four_sfpp_name[OEM_INFO_BUFSIZ];/*4SFP+ card name*/
}oem_info_t;

char* oem_info_get_company_name(char *buf);
char* oem_info_get_package_name(char *buf);
char* oem_info_get_product_name(char *buf);
char* oem_info_get_hardware_type(char *buf);
char* oem_info_get_snmp_enterprise_oid(char *buf);
char* oem_info_get_chassis_type(char *buf);
int oem_info_get_single_snmp_enterprise_oid();
int oem_info_get_oem_vendor_type();
int oem_encode_oem_info(char buffer[], oem_info_t *oem_info);
int oem_decode_oem_info(char buffer[], oem_info_t *oem_info);
int oem_create_oem_info_file(oem_info_t *oem_info);
char* oem_info_get_4sfp_card_name(char* buf);
char* oem_info_get_2sfpp_card_name(char* buf);
char* oem_info_get_4sfpp_card_name(char* buf);

#endif /* _OEM_INFO_H */
