/* ****************************************************************************
 * vim:set shiftwidth=2 softtabstop=2 expandtab:
 * $Id: counterdump.c 5455 2009-05-05 18:18:16Z g9coving $
 *
 * Module:  counterdump.c
 * Project: NetFPGA NIC
 * Description: dumps the MAC Rx/Tx counters to stdout
 * Author: Jad Naous
 *
 * Change history:
 *
 */
/*Testa escrita e leitura na SRAM utilizando interfaces de registradores*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include <net/if.h>

#include "../lib/C/reg_defines_novo_reference_nic.h"
#include "../../../lib/C/common/nf2.h"
#include "../../../lib/C/common/nf2util.h"

#define PATHLEN		80

#define DEFAULT_IFACE	"nf2c0"

/* Global vars */
static struct nf2device nf2;

/*Dports will be record in memory to firewall drop pkts with them*/
uint16_t pdrop[4] = {1010, 80, 22, 667};

/* Function declarations */
void dumpCounts();
void processArgs (int , char **);
void usage (void);

int main(int argc, char *argv[])
{
  nf2.device_name = DEFAULT_IFACE;

  processArgs(argc, argv);

  // Open the interface if possible
  if (check_iface(&nf2))
    {
      exit(1);
    }
  if (openDescriptor(&nf2))
    {
      exit(1);
    }

  dumpCounts();

  closeDescriptor(&nf2);

  return 0;
}

void dumpCounts()
{
  unsigned val;
  /*writeReg(&nf2, SIMULACAO_RD_0_DATA_REG, 0x45460);
  readReg(&nf2, SIMULACAO_RD_0_DATA_REG, &val);
  printf("EscritaPosicaoSRAM:            %x\n\n", val);
  writeReg(&nf2, SIMULACAO_RD_1_DATA_REG, 0x89890);
  writeReg(&nf2, SIMULACAO_TUPLE_PDST_GEN_REG, 0x0);
  writeReg(&nf2, SIMULACAO_TUPLE_IPSRC_GEN_REG, 0x0);
  writeReg(&nf2, 0x100000c, 0x00001234);*/

  /* O sram_arbiter segue a seguinte regra: Os 4 MSB enviados
 * como valor em writeReg dizem qual conjunto de 16bits 
 * da memoria receberá o dado nos 16 LSB do valor. 
 * */ 
  writeReg(&nf2, SRAM_BASE_ADDR,0x4<<28|pdrop[0]);
  writeReg(&nf2, SRAM_BASE_ADDR,0x3<<28|pdrop[1]);
  writeReg(&nf2, SRAM_BASE_ADDR,0x2<<28|pdrop[2]);
  writeReg(&nf2, SRAM_BASE_ADDR,0x1<<28|pdrop[3]);
  sleep(1);
  /*A memória endereça conjuntos de 9bits (9it) por bit:
 * SRAM_BASE_ADDR -> primeiro 9it. SRAM_BASE_ADDR+0x1->
 * segundo 9it, etc. Os 36 bits da memória são organizados 
 * em 32 bits pelo sram_arbiter.v para ser lido em val.
 * */
  readReg(&nf2, (SRAM_BASE_ADDR), &val);
  printf("SRAM %x: %x\n",SRAM_BASE_ADDR,val);
  readReg(&nf2, (SRAM_BASE_ADDR)+4, &val);
  printf("SRAM %x: %x\n",SRAM_BASE_ADDR+4,val);
  /*readReg(&nf2, MAC_GRP_0_RX_QUEUE_NUM_PKTS_STORED_REG, &val);
  printf("Num pkts received on port 0:           %u\n", val);
  readReg(&nf2, MAC_GRP_0_RX_QUEUE_NUM_PKTS_DROPPED_FULL_REG, &val);
  printf("Num pkts dropped (rx queue 0 full):    %u\n", val);
  readReg(&nf2, MAC_GRP_0_RX_QUEUE_NUM_PKTS_DROPPED_BAD_REG, &val);
  printf("Num pkts dropped (bad fcs q 0):        %u\n", val);
  readReg(&nf2, MAC_GRP_0_RX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes received on port 0:          %u\n", val);
  readReg(&nf2, MAC_GRP_0_TX_QUEUE_NUM_PKTS_SENT_REG, &val);
  printf("Num pkts sent from port 0:             %u\n", val);
  readReg(&nf2, MAC_GRP_0_TX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes sent from port 0:            %u\n\n", val);

  readReg(&nf2, MAC_GRP_1_RX_QUEUE_NUM_PKTS_STORED_REG, &val);
  printf("Num pkts received on port 1:           %u\n", val);
  readReg(&nf2, MAC_GRP_1_RX_QUEUE_NUM_PKTS_DROPPED_FULL_REG, &val);
  printf("Num pkts dropped (rx queue 1 full):    %u\n", val);
  readReg(&nf2, MAC_GRP_1_RX_QUEUE_NUM_PKTS_DROPPED_BAD_REG, &val);
  printf("Num pkts dropped (bad fcs q 1):        %u\n", val);
  readReg(&nf2, MAC_GRP_1_RX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes received on port 1:          %u\n", val);
  readReg(&nf2, MAC_GRP_1_TX_QUEUE_NUM_PKTS_SENT_REG, &val);
  printf("Num pkts sent from port 1:             %u\n", val);
  readReg(&nf2, MAC_GRP_1_TX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes sent from port 1:            %u\n\n", val);

  readReg(&nf2, MAC_GRP_2_RX_QUEUE_NUM_PKTS_STORED_REG, &val);
  printf("Num pkts received on port 2:           %u\n", val);
  readReg(&nf2, MAC_GRP_2_RX_QUEUE_NUM_PKTS_DROPPED_FULL_REG, &val);
  printf("Num pkts dropped (rx queue 2 full):    %u\n", val);
  readReg(&nf2, MAC_GRP_2_RX_QUEUE_NUM_PKTS_DROPPED_BAD_REG, &val);
  printf("Num pkts dropped (bad fcs q 2):        %u\n", val);
  readReg(&nf2, MAC_GRP_2_RX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes received on port 2:          %u\n", val);
  readReg(&nf2, MAC_GRP_2_TX_QUEUE_NUM_PKTS_SENT_REG, &val);
  printf("Num pkts sent from port 2:             %u\n", val);
  readReg(&nf2, MAC_GRP_2_TX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes sent from port 2:            %u\n\n", val);

  readReg(&nf2, MAC_GRP_3_RX_QUEUE_NUM_PKTS_STORED_REG, &val);
  printf("Num pkts received on port 3:           %u\n", val);
  readReg(&nf2, MAC_GRP_3_RX_QUEUE_NUM_PKTS_DROPPED_FULL_REG, &val);
  printf("Num pkts dropped (rx queue 3 full):    %u\n", val);
  readReg(&nf2, MAC_GRP_3_RX_QUEUE_NUM_PKTS_DROPPED_BAD_REG, &val);
  printf("Num pkts dropped (bad fcs q 3):        %u\n", val);
  readReg(&nf2, MAC_GRP_3_RX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes received on port 3:          %u\n", val);
  readReg(&nf2, MAC_GRP_3_TX_QUEUE_NUM_PKTS_SENT_REG, &val);
  printf("Num pkts sent from port 3:             %u\n", val);
  readReg(&nf2, MAC_GRP_3_TX_QUEUE_NUM_BYTES_PUSHED_REG, &val);
  printf("Num bytes sent from port 3:            %u\n\n", val);*/
}

/*
 *  Process the arguments.
 */
void processArgs (int argc, char **argv )
{
  char c;

  /* don't want getopt to moan - I can do that just fine thanks! */
  opterr = 0;

  while ((c = getopt (argc, argv, "i:h")) != -1)
    {
      switch (c)
	{
	case 'i':	/* interface name */
	  nf2.device_name = optarg;
	  break;
	case '?':
	  if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr,
		     "Unknown option character `\\x%x'.\n",
		     optopt);
	case 'h':
	default:
	  usage();
	  exit(1);
	}
    }
}


/*
 *  Describe usage of this program.
 */
void usage (void)
{
  printf("Usage: ./counterdump <options> \n\n");
  printf("Options: -i <iface> : interface name (default nf2c0)\n");
  printf("         -h : Print this message and exit.\n");
}
