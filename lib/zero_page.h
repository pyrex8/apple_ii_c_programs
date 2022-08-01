
// Addresses
enum Zero_page
{
    DATA1 = 0x26,
    DATA2,
    DATA3,
    DATA4,
    DATA5,

    ADDR1L,
    ADDR1H,
    ADDR2L,
    ADDR2H,

    LKLOL,
    LKLOH,
    LKHIL,
    LHHIH,

    SBUFRL,
    SBUFRH,
};

// low byte is used for some instructions, alias so that it maps to assembly version
#define LKLO                LKLOL
#define LKHI                LKHIL
#define SBUFR               SBUFRL // sprint buffer location

// pointers to zero page memory
#define DATA1_P             *((uint8_t*)DATA1)
#define DATA2_P             *((uint8_t*)DATA2)
#define DATA3_P             *((uint8_t*)DATA3)
#define DATA4_P             *((uint8_t*)DATA4)
#define DATA5_P             *((uint8_t*)DATA5)

#define ADDR1L_P            *((uint8_t*)ADDR1L)
#define ADDR1H_P            *((uint8_t*)ADDR1H)
#define ADDR2L_P            *((uint8_t*)ADDR2L)
#define ADDR2H_P            *((uint8_t*)ADDR2H)

#define LKLOL_P             *((uint8_t*)LKLOL)
#define LKLOH_P             *((uint8_t*)LKLOH)
#define LKHIL_P             *((uint8_t*)LKHIL)
#define LKHIH_P             *((uint8_t*)LHHIH)

#define SBUFRL_P            *((uint8_t*)SBUFRL)
#define SBUFRH_P            *((uint8_t*)SBUFRH)
