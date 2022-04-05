#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VPN_MASK 0xFFFFF000
#define OFFSET_MASK 0xFFF
#define SHIFT 12
#define FPN_SHIFT ;
struc {
  int valid;
  int protectedBits
  // otros flags
}
PTE;
// Retorna la VPN
int get_vpn_index(void *va) { return (va & VPN_MASK) >> SHIFT; }
// retorna la dirección de PTE
void *get_pte_address(void *reg_base_PT, int vpn_index) {
  return reg_base_PT + (vpn_index * sizeof(PTE));
}
int validarBits(PTE pte) {
  // validar permisos
  // retorna 1 si tiene permisos y valid = 1
  return 1;
}
void *get_physical_address(void *va, PTE pte) {
  if (validarBits(pte)) {
    int *offset;
    offset = va & OFFSET_MASK;
    return (pte.FPN << SHIFT) | offset;
  }
}
int main() {
  /*
  32 bits de address space
  4kb de page size.
  ==============================
  4kb = 4096 bytes = 2^(12) = 12 bits de offset
  por lo tanto 20 bits para VPN.
  */
  char *base_reg;
  char *hello = "hOLLA";
  char *va_test = &hello;
  char *pa_test;
  PTE pte;
  char *pte_addr;
  int index = get_vpn_index(va_test);
  pte_addr = get_pte_address(base_reg, index);
  pte = (PTE)(&pte_addr);
  pa_test = get_physical_address(va_test, pte);
  printf("La dirección es: %x \n", pa_test);
  return 0;
}
