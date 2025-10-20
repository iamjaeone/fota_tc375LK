#ifndef ABSWAP_H
#define ABSWAP_H

#include "Ifx_Types.h"

void ABSwap_Swap();
void ABSwap_InitializeUCBSwap();
void setSwapConfiguration(uint32 ucbSwapOffsetToWrite, boolean swapToAlternate);
void restoreSwapConfig();

void writeSwapConfig(uint32 index);

void eraseSwapConfig();


void eraseLastSwapConfig();
#endif /* ABSWAP_H */