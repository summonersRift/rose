
#include "KLT/RTL/data.h"

int klt_same_data(struct klt_data_t * d1, struct klt_data_t * d2) {
  if ((d1->ptr == d2->ptr) && (d1->num_sections == d2->num_sections)) {
    size_t i;
    for (i = 0; i < d1->num_sections; i++)
      if (d1->sections[i].length != d2->sections[i].length)
        return 1;
    return 0;
  }
  return 1;
}

