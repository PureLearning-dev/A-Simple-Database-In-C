import subprocess

# ==============================================
# 工具函数：运行 C 语言数据库程序
# ==============================================
def run_script(commands: list[str]) -> list[str]:
    process = subprocess.Popen(
        ["./db"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    input_data = "\n".join(commands) + "\n"
    output, _ = process.communicate(input_data)
    return [line.rstrip() for line in output.strip().splitlines()]

# ==============================================
# 测试用例
# ==============================================
def test_insert_and_select():
    result = run_script([
        "insert 1 user1 person1@example.com",
        "select",
        ".exit",
    ])

    expected = [
        "db ~> Executed.",
        "db ~> (1, user1, person1@example.com)",
        "Executed.",
        "db ~>"
    ]

    assert result == expected, "❌ 测试失败：insert + select"
    print("✅ test_insert_and_select 测试通过！")

def test_insert_max_length_strings():
    long_username = "a" * 32
    long_email = "a" * 255

    result = run_script([
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit",
    ])

    expected = [
        "db ~> Executed.",
        f"db ~> (1, {long_username}, {long_email})",
        "Executed.",
        "db ~>"
    ]

    assert result == expected, "❌ 测试失败：最大长度字符串插入"
    print("✅ test_insert_max_length_strings 测试通过！")

def test_negative_id_error():
    result = run_script([
        "insert -1 cstack foo@bar.com",
        "select",
        ".exit",
    ])

    expected = [
        "db ~> ID must be positive.",
        "db ~> Executed.",
        "db ~>"
    ]

    assert result == expected, "❌ 测试失败：负数 ID 检查"
    print("✅ test_negative_id_error 测试通过！")

# ==============================================
# 运行所有测试
# ==============================================
if __name__ == "__main__":
    try:
        test_insert_and_select()
        test_insert_max_length_strings()
        test_negative_id_error()
        print("\n🎉 所有测试全部通过！")
    except AssertionError as e:
        print(f"\n{e}")