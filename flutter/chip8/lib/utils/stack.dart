// Implementation Of Stack In Dart

class MemoryStack {
  final List<int> _stack = [];

  void push(int data) {
    if (_stack.length >= 16) return;
    _stack.insert(_stack.length - 1, data);
  }

  int pop() {
    if (_stack.isEmpty) return 0;
    return _stack.removeLast();
  }

  int sp() {
    return _stack.length;
  }
}
