printf("stretch tree of depth %u\t check: %li\n", stretchDepth, stretchTree.ItemCheck());


#include #include
1234 56789 -1.2345e7 (111, 222, 333) == --
0x1.234
0x.1234
1234
0x1234
0b1110
1e-1
1e100
1234 56789 1.2345e7 == --


// single line comment

/* multi line comment
of stuff and stuff */

"""one liner raw string"""
"""
totally raw
string of stuff
and stuff and stuff
"""

'''dGhpcyB0ZXh0IHdhcyBiYXNlNjQgZW5jb2RlZA=='''
'''dGhpcyB0ZXh0IHdhcyBiYXNlNjQgZW5jb2Rl'''
'''dGhpcyB0ZXh0IHdhcyBiYXNlNjQgZW5jb2Q='''
'''dGhpcyB0ZXh0IHdhcyBiYXNlNjQgZW5jbw=='''
'''dGhpcyB0ZXh0IHdhcyBiYXNlNjQgZW5j'''
'''
dGVzdA==
'''
'''
VGhlIHBhcnRpY3VsYXIgY2hvaWNlIG9mIGNoYXJhY3RlciBzZXQgc2VsZWN0ZWQgZm9yIHRoZSA2
NCBjaGFyYWN0ZXJzIHJlcXVpcmVkIGZvciB0aGUgYmFzZSB2YXJpZXMgYmV0d2VlbiBpbXBsZW1l
bnRhdGlvbnMuIFRoZSBnZW5lcmFsIHJ1bGUgaXMgdG8gY2hvb3NlIGEgc2V0IG9mIDY0IGNoYXJh
Y3RlcnMgdGhhdCBpcyBib3RoIHBhcnQgb2YgYSBzdWJzZXQgY29tbW9uIHRvIG1vc3QgZW5jb2Rp
bmdzLCBhbmQgYWxzbyBwcmludGFibGUuIFRoaXMgY29tYmluYXRpb24gbGVhdmVzIHRoZSBkYXRh
IHVubGlrZWx5IHRvIGJlIG1vZGlmaWVkIGluIHRyYW5zaXQgdGhyb3VnaCBpbmZvcm1hdGlvbiBz
eXN0ZW1zLCBzdWNoIGFzIGVtYWlsLCB0aGF0IHdlcmUgdHJhZGl0aW9uYWxseSBub3QgOC1iaXQg
Y2xlYW4uWzFdIEZvciBleGFtcGxlLCBNSU1FJ3MgQmFzZTY0IGltcGxlbWVudGF0aW9uIHVzZXMg
QS1aLCBhLXosIGFuZCAwLTkgZm9yIHRoZSBmaXJzdCA2MiB2YWx1ZXMuIE90aGVyIHZhcmlhdGlv
bnMsIHVzdWFsbHkgZGVyaXZlZCBmcm9tIEJhc2U2NCwgc2hhcmUgdGhpcyBwcm9wZXJ0eSBidXQg
ZGlmZmVyIGluIHRoZSBzeW1ib2xzIGNob3NlbiBmb3IgdGhlIGxhc3QgdHdvIHZhbHVlczsgYW4g
ZXhhbXBsZSBpcyBVVEYtNy4=
'''

//-----------------------------------------------------------------------------

TreeNode = Struct(item : int, left : TreeNode?, right : TreeNode?);


TreeNode.ItemCheck = function(this : TreeNode) -> int {
  if (this.left && this.right) {
    result = this.item + left.ItemCheck() - right.ItemCheck();
  } else {
    result = this.item;
  }
}

TreeNode.BottomUpTree = function(item : int, depth : int) -> TreeNode {
  if (depth > 0) {
    result = TreeNode(item,
                      BottomUpTree(2 * item - 1, depth - 1),
                      BottomUpTree(2 * item, depth - 1));
  } else {
    result = TreeNode(item, NULL, NULL);
  }
}

//-----------------------------------------------------------------------------

main = function(argc : int, argv : List(string)) {
  minDepth = 4;
  maxDepth = max(toInt(argv[1]), 6);
  stretchDepth = maxDepth + 1;

  stretchTree = TreeNode.BottomUpTree(0, stretchDepth);
  printf("stretch tree of depth %u\t check: %li\n", stretchDepth, stretchTree.ItemCheck());
  delete stretchTree;

  longLivedTree = Tree.BottomUpTree(0, maxDepth);

  for (depth = minDepth, depth <= maxDepth, depth + 2) {
      iterations = pow(2, maxDepth - depth + minDepth);
      check = sum(i, range(1, iterations)) {
          tempTree1 = TreeNode.BottomUpTree(i, depth);
          tempTree2 = TreeNode.BottomUpTree(-i, depth);
          result = tempTree1.ItemCheck() + tempTree2.ItemCheck();
      }

      printf("%li\t trees of depth %u\t check: %li\n", iterations * 2, depth, check);
  }

  printf("long lived tree of depth %u\t check: %li\n", maxDepth, longLivedTree.ItemCheck());
  result = 0;
}

//-----------------------------------------------------------------------------

sum = function(range : Generator(int)) context block {
  result = reduce(0, range, lambda(context, block));
}

range = function(? : int = #, min : int, max : int) context block {
  param = (name(args[0]) : int);
  for (i, min, max) {
    exec(context, block, param = i);
  }
}


range = Generator(min : int, max : int) -> int {
  loop (i : int = min) {
    yield i;
    if (i == max) break;
    result = (i : int = i + 1);
  }
}


//-----------------------------------------------------------------------------


for = localfunction(#cursor, terminator : Expression, next : Expression) block {
  loop (#cursor) {
    if (!eval(context, terminator, cursor)) break;
    exec(context, block, cursor);
    result = eval(context, next, cursor);
  }
}

for (depth = minDepth, depth <= maxDepth, depth + 2) {
    iterations = pow(2, maxDepth - depth + minDepth);
    check = sum(i, range(1, iterations)) {
        tempTree1 = TreeNode.BottomUpTree(i, depth);
        tempTree2 = TreeNode.BottomUpTree(-i, depth);
        result = tempTree1.ItemCheck() + tempTree2.ItemCheck();
    }

    printf("%li\t trees of depth %u\t check: %li\n", iterations * 2, depth, check);
}

//-----------------------------------------------------------------------------



"this is a string with an embedded \" quote character"

"this is a string with a \
newline continuation in it."

"this string has a null right here - \0 - so this should not be printed"