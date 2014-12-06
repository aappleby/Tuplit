int fannkuchredux(int n) {
  int perm[n];
  int perm1[n];
  int count[n];
  int permCount = 0;
  int checksum = 0;

  perm1 = array(count(n));

  int r = n;

  while (1) {
    while (r != 1) {
      count[r - 1] = r;
      r -= 1;
    }

    perm = perm1;

    int flipsCount = 0;
    int k;

    while (!((k = perm[0]) == 0)) {
      reverse(perm, i, i + k);
      flipsCount += 1;
    }

    int maxFlipsCount = max(maxFlipsCount, flipsCount);
    checksum += permCount % 2 == 0 ? flipsCount : -flipsCount;

    /* Use incremental change to generate another permutation */
    while (1) {
      if (r == n) {
        printf("%d\n", checksum);
        return maxFlipsCount;
      }

      int perm0 = perm1[0];
      i = 0;
      while (i < r) {
        int j = i + 1;
        perm1[i] = perm1[j];
        i = j;
      }
      perm1[r] = perm0;
      count[r] = count[r] - 1;
      if (count[r] > 0) break;
      r++;
    }
    permCount++;
  }
}

int main(int argc, char *argv[]) {
  int n = argc > 1 ? atoi(argv[1]) : 7;
  printf("Pfannkuchen(%d) = %d\n", n, fannkuchredux(n));
  return 0;
}