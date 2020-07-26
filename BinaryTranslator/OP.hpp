#ifndef LANG_OP_CPP
#define LANG_OP_CPP

#include <string>
#include <iostream>

using std::string;
// TODO: r8-r15 are not supported

#define REX_PREFIX 0b01001000

#define RAX 0b000
#define RCX 0b001
#define RDX 0b010
#define RBX 0b011
#define RSP 0b100
#define RBP 0b101
#define RSI 0b110
#define RDI 0b111


using IMM8 = int8_t;
using IMM32 = int32_t;

class REG {
public:
  REG(size_t ID, int offset = 0, bool is_addr = false) : ID_(ID), offset_(offset), is_addr_(offset != 0 || is_addr) {}
  uint8_t GetId() const { return ID_; }
  REG& operator [](int a) {
    offset_ = a;
    return *this;
  }
  bool IsAddr() const { return is_addr_; }
  int  GetOff() const { return offset_; }
private:
  uint8_t ID_;
  int offset_;
  bool is_addr_;
};

class Instruction {
public:
  Instruction() = default;

  // Handle REG REG, R/M64 REG, REG R/M64.
  Instruction(const REG& to, const REG& from) : rex_prefix(1, REX_PREFIX) {
    unsigned char result = 0b11000000;
    if (to.IsAddr()) {
      if (to.GetOff() == 0) {
        result = 0b00000000;
      }
      else if (to.GetOff() > -128 && to.GetOff() < 128) {
        result = 0b01000000;
        disp = std::string(1, to.GetOff());
      }
      else
        result = 0b10000000;
    }
    if (from.IsAddr()) {
      if (from.GetOff() == 0) {
        result = 0b00000000;
      }
      else if (from.GetOff() > -128 && from.GetOff() < 128) {
        result = 0b01000000;
        disp = std::string(1, from.GetOff());
      }
      else {
        result = 0b10000000;
        // TODO: 1 byte, 2 byte, 4 bytes to right string version
      }
    }
    if (from.IsAddr())
      result += (to.GetId() << 3) + from.GetId();
    else
      result += (from.GetId() << 3) + to.GetId();
    ModRM = std::string(1, result);
  }

  // Handle R/M64, IMM64
  Instruction(const REG& to, IMM32 val) : rex_prefix(1, REX_PREFIX) {
    unsigned char result = 0b11000000;
    result += to.GetId();
    ModRM = std::string(1, result);
    for (int i = 0; i < 4; ++i) {
      imm += std::string(1, char(val & 0b11111111));
      val >>= 8;
    }
  }

  std::string Get() { return rex_prefix + opcode + ModRM + disp + imm; }
protected:
  std::string rex_prefix;
  std::string opcode;
  std::string ModRM;
  std::string disp;
  std::string imm;
};

class MOV : public Instruction {
public:
  MOV(const REG& to, const REG& from) : Instruction(to, from) {
    if (from.IsAddr())
      opcode = std::string(1, 0x8b);
    else
      opcode = std::string(1, 0x89);
  }
  MOV(const REG& to, IMM32 val) : Instruction(to, val) {
    opcode = std::string(1, 0xc7);
  }
};

class PUSH : public Instruction {
public:
  PUSH(const REG& op) {
    if (!op.IsAddr()) {
      char result = 0x50 + op.GetId();
      opcode = std::string(1, result);
    } else {
      opcode = std::string(1, 0xff);
      if (-128 < op.GetOff() && op.GetOff() < 128) {
        ModRM = std::string(1, 0x70 + op.GetId());
        disp = std::string(1, op.GetOff());
      } else {
        ModRM = std::string(1, 0xb0 + op.GetId());
        int off = op.GetOff();
        for (int i = 0; i < 4; ++i) {
          disp += std::string(1, char(off & 0b11111111));
          off >>= 8;
        }
      }
    }
  }

  PUSH(IMM8 num) {
    opcode = {0x6a};
    imm = std::string(1, num);
  }
};

class POP : public Instruction {
public:
  POP(const REG& op) {
    if (!op.IsAddr()) {
      char result = 0x58 + op.GetId();
      opcode = std::string(1, result);
      return;
    } else {
      opcode = std::string(1, 0x8f);
      if (-128 < op.GetOff() && op.GetOff() < 128) {
        ModRM = std::string(1, 0b01000000 + op.GetId());
        disp = std::string(1, op.GetOff());
      }
      else {
        ModRM = std::string(1, 0b10000000 + op.GetId());
        int off = op.GetOff();
        for (int i = 0; i < 4; ++i) {
          disp += std::string(1, char(off & 0b11111111));
          off >>= 8;
        }
      }
    }
  }
};

class CMP : public Instruction {
public:
  CMP(const REG& left, const REG& right) : Instruction(left, right) {
    opcode = std::string(1, 0x39);
  }
};

class SYSCALL : public Instruction {
public:
  explicit SYSCALL() {
    opcode = {0x0F, 0x05};
  }
};

class JMP : public Instruction {
public:
  explicit JMP(IMM8 off) {
    opcode = {char(0xeb)};
    disp = {off};
  }
};

class JE : public Instruction {
public:
  explicit JE(IMM8 off) {
    opcode = {char(0x74)};
    disp = {off};
  }
};

class CALL : public Instruction {
public:
  explicit CALL(IMM32 off) {
    opcode = {char(0xe8)};
    for (int i = 0; i < 4; ++i) {
      imm += std::string(1, char(off & 0b11111111));
      off >>= 8;
    }
  }
};

class RET : public Instruction {
public:
  explicit RET() {
    opcode = {char(0xc3)};
  }
};

class SUB : public Instruction {
public:
  explicit SUB(const REG& reg, IMM32 val) {
    rex_prefix = std::string(1, REX_PREFIX);
    opcode = {char(0x81)};
    ModRM = std::string(1, 0xe8 + reg.GetId());
    for (int i = 0; i < 4; ++i) {
      imm += std::string(1, char(val & 0b11111111));
      val >>= 8;
    }
  }
};

void print(const std::string& a) {
  for (auto i : a) {
    printf("%02x ", uint8_t(i));
  }
  printf("\n");
}

#endif //LANG_OP_CPP
