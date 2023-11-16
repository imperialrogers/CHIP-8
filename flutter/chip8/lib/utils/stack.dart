// Implementation Of Stack In Dart
import 'dart:collection';

class MemoryStack {
  Queue stack = new Queue<int>();

  int pop() {
    if (this.stack.length == 0) return 0;
    return stack.removeLast();
  }

  push(int index) {
    if (this.stack.length >= 16) {
      return;
    }
    this.stack.addLast(index);
  }

  int sp() {
    return this.stack.length;
  }
}
