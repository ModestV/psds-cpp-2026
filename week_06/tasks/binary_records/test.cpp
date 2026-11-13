#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "binary_records.cpp"


class BinaryRecordsTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        path_ = std::filesystem::temp_directory_path() /
                (std::string("psds_binary_records_") + info->test_suite_name() + "_" +
                 info->name() + ".bin");
        std::filesystem::remove(path_);
    }

    void TearDown() override {
        std::filesystem::remove(path_);
    }

    static constexpr std::size_t HEADER_SIZE = 10;
    static constexpr std::size_t SCORE_OFFSET_0 = 19;
    static constexpr std::size_t SCORE_OFFSET_1 = 29;
    static constexpr std::size_t RECORD_OVERHEAD = 10;

    inline static const std::vector<Student> STUDENTS = {
        {1, "Ann", 95},
        {258, "", -1},
        {0xDEADBEEF, "Иван Петров", std::numeric_limits<std::int32_t>::min()},
        {7, std::string("a\0b", 3), std::numeric_limits<std::int32_t>::max()},
    };

    // {1, "Ann", 95}, {258, "", -1}
    inline static const std::string BASIC_EXAMPLE = std::string(
        "PSDS"
        "\x01\x00"
        "\x02\x00\x00\x00"
        "\x01\x00\x00\x00" "\x03\x00" "Ann" "\x5F\x00\x00\x00"
        "\x02\x01\x00\x00" "\x00\x00" "\xFF\xFF\xFF\xFF",
        33);

    static std::string Write(const std::vector<Student>& students) {
        std::ostringstream out(std::ios::binary);
        EXPECT_TRUE(WriteStudents(out, students));
        return out.str();
    }

    static bool Read(const std::string& data, std::vector<Student>& students) {
        std::istringstream in(data, std::ios::binary);
        return ReadStudents(in, students);
    }

    static std::vector<Student> ReadOk(const std::string& data) {
        std::vector<Student> students;
        EXPECT_TRUE(Read(data, students));
        return students;
    }

    static std::int64_t Count(const std::string& data) {
        std::istringstream in(data, std::ios::binary);
        return CountStudents(in);
    }

    static std::stringstream MakeStream(const std::string& data) {
        return std::stringstream(data, std::ios::in | std::ios::out | std::ios::binary);
    }

    static std::string WithScore(std::string data, std::size_t offset, std::uint32_t score) {
        for (int i = 0; i < 4; ++i) {
            data[offset + i] = static_cast<char>((score >> (8 * i)) & 0xFF);
        }
        return data;
    }

    static std::size_t FileSize(const std::vector<Student>& students) {
        std::size_t size = HEADER_SIZE;
        for (const Student& s : students) {
            size += RECORD_OVERHEAD + s.name.size();
        }
        return size;
    }

    // std::mt19937 sequence is fully specified by the standard, so the data
    // is the same on every platform. Distributions are not, so they are not used
    static std::vector<Student> Generate(std::size_t count, std::uint32_t seed) {
        std::mt19937 gen(seed);
        std::vector<Student> students(count);
        for (std::size_t i = 0; i < count; ++i) {
            students[i].id = static_cast<std::uint32_t>(i) * 2654435761u;  // unique ids
            students[i].name.resize(gen() % 300);
            for (char& c : students[i].name) {
                c = static_cast<char>(gen() & 0xFF);
            }
            students[i].score = static_cast<std::int32_t>(gen());
        }
        return students;
    }

    void WriteFile(const std::string& data) const {
        std::ofstream(path_, std::ios::binary)
            .write(data.data(), static_cast<std::streamsize>(data.size()));
    }

    std::string ReadFile() const {
        std::ifstream file(path_, std::ios::binary);
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    std::fstream OpenFile() const {
        return std::fstream(path_, std::ios::in | std::ios::out | std::ios::binary);
    }

    std::filesystem::path path_;
};

class WriteStudentsTest : public BinaryRecordsTest {};
class ReadStudentsTest : public BinaryRecordsTest {};
class CountStudentsTest : public BinaryRecordsTest {};
class UpdateScoreTest : public BinaryRecordsTest {};
class FileTest : public BinaryRecordsTest {};
class LargeDataTest : public BinaryRecordsTest {};

TEST_F(WriteStudentsTest, GoldenBytes) {
    EXPECT_EQ(Write({STUDENTS[0], STUDENTS[1]}), BASIC_EXAMPLE);
}

TEST_F(WriteStudentsTest, Empty) {
    EXPECT_EQ(Write({}), std::string("PSDS\x01\x00\x00\x00\x00\x00", 10));
}

TEST_F(WriteStudentsTest, NameLengthInBytes) {
    const std::string data = Write({{1, "Иван", 0}});
    ASSERT_EQ(data.size(), FileSize({{1, "Иван", 0}}));
    EXPECT_EQ(data[14], '\x08');
    EXPECT_EQ(data[15], '\x00');
}

TEST_F(WriteStudentsTest, AppendsAtCurrentPosition) {
    std::ostringstream out(std::ios::binary);
    out << "prefix";
    EXPECT_TRUE(WriteStudents(out, {STUDENTS[0], STUDENTS[1]}));
    EXPECT_EQ(out.str(), "prefix" + BASIC_EXAMPLE);
}

TEST_F(WriteStudentsTest, MaxNameLength) {
    const std::vector<Student> students = {{1, std::string(65535, 'x'), 2}};
    const std::string data = Write(students);
    EXPECT_EQ(data.size(), FileSize(students));
    EXPECT_EQ(ReadOk(data), students);
}

TEST_F(WriteStudentsTest, TooLongName) {
    std::ostringstream out(std::ios::binary);
    EXPECT_FALSE(WriteStudents(out, {STUDENTS[0], {2, std::string(65536, 'x'), 0}}));
    EXPECT_EQ(out.str(), "") << "nothing should be written";
}

TEST_F(WriteStudentsTest, BadStream) {
    std::ostringstream out(std::ios::binary);
    out.setstate(std::ios::badbit);
    EXPECT_FALSE(WriteStudents(out, STUDENTS));
}

TEST_F(ReadStudentsTest, GoldenBytes) {
    EXPECT_EQ(ReadOk(BASIC_EXAMPLE), (std::vector<Student>{STUDENTS[0], STUDENTS[1]}));
}

TEST_F(ReadStudentsTest, RoundTrip) {
    EXPECT_EQ(ReadOk(Write(STUDENTS)), STUDENTS);
}

TEST_F(ReadStudentsTest, Empty) {
    std::vector<Student> students = STUDENTS;
    EXPECT_TRUE(Read(Write({}), students));
    EXPECT_TRUE(students.empty());
}

TEST_F(ReadStudentsTest, ReplacesContent) {
    std::vector<Student> students = STUDENTS;
    EXPECT_TRUE(Read(BASIC_EXAMPLE, students));
    EXPECT_EQ(students.size(), 2u);
}

TEST_F(ReadStudentsTest, ReadsFromCurrentPosition) {
    std::istringstream in("xyz" + BASIC_EXAMPLE, std::ios::binary);
    in.seekg(3);
    std::vector<Student> students;
    EXPECT_TRUE(ReadStudents(in, students));
    EXPECT_EQ(students.size(), 2u);
}

TEST_F(ReadStudentsTest, BadMagic) {
    std::string data = BASIC_EXAMPLE;
    data[3] = 'X';
    std::vector<Student> students = STUDENTS;
    EXPECT_FALSE(Read(data, students));
    EXPECT_EQ(students, STUDENTS);
    EXPECT_FALSE(Read("psds" + BASIC_EXAMPLE.substr(4), students));
}

TEST_F(ReadStudentsTest, BadVersion) {
    std::string data = BASIC_EXAMPLE;
    data[4] = '\x02';
    std::vector<Student> students = STUDENTS;
    EXPECT_FALSE(Read(data, students));
    data[4] = '\x01';
    data[5] = '\x01';  // 257
    EXPECT_FALSE(Read(data, students));
    EXPECT_EQ(students, STUDENTS);
}

TEST_F(ReadStudentsTest, EveryTruncationFails) {
    const std::string data = Write(STUDENTS);
    for (std::size_t size = 0; size < data.size(); ++size) {
        std::vector<Student> students = {STUDENTS[0]};
        EXPECT_FALSE(Read(data.substr(0, size), students)) << "size " << size;
        EXPECT_EQ(students, std::vector<Student>{STUDENTS[0]}) << "size " << size;
    }
}

TEST_F(ReadStudentsTest, ExtraBytesFail) {
    std::vector<Student> students;
    EXPECT_FALSE(Read(BASIC_EXAMPLE + '\0', students));
    EXPECT_FALSE(Read(Write({}) + "x", students));
    EXPECT_TRUE(students.empty());
}

TEST_F(ReadStudentsTest, WrongCount) {
    std::string data = BASIC_EXAMPLE;
    data[6] = '\x01';  // one record, but there are two
    std::vector<Student> students;
    EXPECT_FALSE(Read(data, students));
    data[6] = '\x03';
    EXPECT_FALSE(Read(data, students));
    data[6] = '\x00';
    data[9] = '\x01';  // 16777216 records
    EXPECT_FALSE(Read(data, students));
}

TEST_F(CountStudentsTest, Valid) {
    EXPECT_EQ(Count(BASIC_EXAMPLE), 2);
    EXPECT_EQ(Count(Write({})), 0);
    EXPECT_EQ(Count(Write(STUDENTS)), 4);
}

TEST_F(CountStudentsTest, ReadsOnlyHeader) {
    EXPECT_EQ(Count(BASIC_EXAMPLE.substr(0, HEADER_SIZE)), 2);
    std::istringstream in(BASIC_EXAMPLE, std::ios::binary);
    EXPECT_EQ(CountStudents(in), 2);
    EXPECT_EQ(in.tellg(), static_cast<std::streamoff>(HEADER_SIZE));
}

TEST_F(CountStudentsTest, LargeCount) {
    std::string data = Write({});
    data[6] = data[7] = data[8] = data[9] = '\xFF';
    EXPECT_EQ(Count(data), 4294967295LL);
}

TEST_F(CountStudentsTest, Invalid) {
    EXPECT_EQ(Count(""), -1);
    EXPECT_EQ(Count("PSDS"), -1);
    EXPECT_EQ(Count(BASIC_EXAMPLE.substr(0, HEADER_SIZE - 1)), -1);
    std::string data = BASIC_EXAMPLE;
    data[0] = 'p';
    EXPECT_EQ(Count(data), -1);
    data = BASIC_EXAMPLE;
    data[4] = '\x00';
    EXPECT_EQ(Count(data), -1);
}

TEST_F(UpdateScoreTest, StringStream) {
    std::stringstream ss = MakeStream(Write(STUDENTS));
    EXPECT_TRUE(UpdateScore(ss, 258, 100));

    std::vector<Student> expected = STUDENTS;
    expected[1].score = 100;
    EXPECT_EQ(ss.str(), Write(expected));
}

TEST_F(UpdateScoreTest, OnlyFourBytesChange) {
    std::stringstream ss = MakeStream(BASIC_EXAMPLE);
    EXPECT_TRUE(UpdateScore(ss, 1, -2));
    EXPECT_EQ(ss.str(), WithScore(BASIC_EXAMPLE, SCORE_OFFSET_0, 0xFFFFFFFE));
    EXPECT_TRUE(UpdateScore(ss, 258, 0x01020304));
    EXPECT_EQ(ss.str(), WithScore(WithScore(BASIC_EXAMPLE, SCORE_OFFSET_0, 0xFFFFFFFE), SCORE_OFFSET_1, 0x01020304));
}

TEST_F(UpdateScoreTest, EveryRecord) {
    std::stringstream ss = MakeStream(Write(STUDENTS));
    std::vector<Student> expected = STUDENTS;
    for (std::size_t i = 0; i < STUDENTS.size(); ++i) {
        expected[i].score = static_cast<std::int32_t>(i) * 1000 - 1;
        EXPECT_TRUE(UpdateScore(ss, STUDENTS[i].id, expected[i].score)) << "id " << STUDENTS[i].id;
    }
    EXPECT_EQ(ss.str(), Write(expected));
}

TEST_F(UpdateScoreTest, StartsFromBeginning) {
    std::stringstream ss = MakeStream(BASIC_EXAMPLE);
    std::vector<Student> students;
    EXPECT_TRUE(ReadStudents(ss, students));  // stream is now at the end, eofbit is set
    EXPECT_TRUE(UpdateScore(ss, 1, 50));
    EXPECT_TRUE(UpdateScore(ss, 1, 60));
    EXPECT_EQ(ss.str(), WithScore(BASIC_EXAMPLE, SCORE_OFFSET_0, 60));

    ss.setstate(std::ios::failbit);
    EXPECT_TRUE(UpdateScore(ss, 1, 70));
    EXPECT_EQ(ss.str(), WithScore(BASIC_EXAMPLE, SCORE_OFFSET_0, 70));
}

TEST_F(UpdateScoreTest, StreamGoodAfterSuccess) {
    std::stringstream ss = MakeStream(BASIC_EXAMPLE);
    EXPECT_TRUE(UpdateScore(ss, 258, 1));
    EXPECT_TRUE(ss.good());
    ss.seekg(0);
    std::vector<Student> students;
    EXPECT_TRUE(ReadStudents(ss, students));
    ASSERT_EQ(students.size(), 2u);
    EXPECT_EQ(students[1].score, 1);
}

TEST_F(UpdateScoreTest, DuplicateIdUpdatesFirst) {
    const std::vector<Student> students = {{5, "first", 1}, {5, "second", 2}};
    std::stringstream ss = MakeStream(Write(students));
    EXPECT_TRUE(UpdateScore(ss, 5, 10));
    EXPECT_EQ(ss.str(), Write({{5, "first", 10}, {5, "second", 2}}));
}

TEST_F(UpdateScoreTest, MissingId) {
    const std::string data = Write(STUDENTS);
    std::stringstream ss = MakeStream(data);
    EXPECT_FALSE(UpdateScore(ss, 2, 100));
    EXPECT_EQ(ss.str(), data);

}

TEST_F(UpdateScoreTest, EmptyFile) {
    std::stringstream ss = MakeStream(Write({}));
    EXPECT_FALSE(UpdateScore(ss, 0, 1));
    EXPECT_EQ(ss.str(), Write({}));
}

TEST_F(UpdateScoreTest, InvalidHeader) {
    std::string data = BASIC_EXAMPLE;
    data[0] = 'X';
    std::stringstream ss = MakeStream(data);
    EXPECT_FALSE(UpdateScore(ss, 1, 100));
    EXPECT_EQ(ss.str(), data);
}

TEST_F(UpdateScoreTest, EveryTruncationKeepsSize) {
    const std::string data = Write(STUDENTS);
    for (std::size_t size = 0; size < data.size(); ++size) {
        const std::string truncated = data.substr(0, size);
        std::stringstream ss = MakeStream(truncated);
        EXPECT_FALSE(UpdateScore(ss, 7, 100)) << "size " << size;
        EXPECT_EQ(ss.str(), truncated) << "size " << size;
    }
}

TEST_F(FileTest, UpdateScoreFileStream) {
    WriteFile(Write(STUDENTS));
    {
        std::fstream file = OpenFile();
        ASSERT_TRUE(file.is_open());
        EXPECT_TRUE(UpdateScore(file, 0xDEADBEEF, 42));
        EXPECT_TRUE(UpdateScore(file, 1, -42));
        EXPECT_FALSE(UpdateScore(file, 3, 0));
    }
    std::vector<Student> expected = STUDENTS;
    expected[0].score = -42;
    expected[2].score = 42;
    EXPECT_EQ(ReadFile(), Write(expected));
}

TEST_F(FileTest, UpdateScoreTruncatedFileKeepsSize) {
    const std::string data = Write(STUDENTS);
    for (std::size_t cut = 1; cut <= 8; ++cut) {
        const std::string truncated = data.substr(0, data.size() - cut);
        WriteFile(truncated);
        {
            std::fstream file = OpenFile();
            ASSERT_TRUE(file.is_open());
            EXPECT_FALSE(UpdateScore(file, 7, 100)) << "cut " << cut;
        }
        EXPECT_EQ(ReadFile(), truncated) << "cut " << cut;
    }
}

TEST_F(FileTest, WriteAndReadFile) {
    {
        std::ofstream out(path_, std::ios::binary);
        ASSERT_TRUE(out.is_open());
        EXPECT_TRUE(WriteStudents(out, STUDENTS));
    }
    EXPECT_EQ(std::filesystem::file_size(path_), Write(STUDENTS).size());
    {
        std::ifstream in(path_, std::ios::binary);
        std::vector<Student> students;
        EXPECT_TRUE(ReadStudents(in, students));
        EXPECT_EQ(students, STUDENTS);
    }
    {
        std::ifstream in(path_, std::ios::binary);
        EXPECT_EQ(CountStudents(in), 4);
    }
}

TEST_F(LargeDataTest, RoundTripStringStream) {
    const std::vector<Student> students = Generate(20000, 2026);
    const std::string data = Write(students);
    EXPECT_EQ(data.size(), FileSize(students));
    EXPECT_EQ(Count(data), 20000);
    EXPECT_EQ(ReadOk(data), students);
}

TEST_F(LargeDataTest, RoundTripFile) {
    const std::vector<Student> students = Generate(20000, 6);
    {
        std::ofstream out(path_, std::ios::binary);
        ASSERT_TRUE(out.is_open());
        EXPECT_TRUE(WriteStudents(out, students));
    }
    EXPECT_EQ(std::filesystem::file_size(path_), FileSize(students));

    std::ifstream in(path_, std::ios::binary);
    std::vector<Student> loaded;
    EXPECT_TRUE(ReadStudents(in, loaded));
    EXPECT_EQ(loaded, students);
}

TEST_F(LargeDataTest, RandomTruncationsFail) {
    const std::vector<Student> students = Generate(2000, 42);
    const std::string data = Write(students);
    std::mt19937 gen(7);
    for (int i = 0; i < 200; ++i) {
        const std::size_t size = gen() % data.size();
        std::vector<Student> loaded = {STUDENTS[0]};
        EXPECT_FALSE(Read(data.substr(0, size), loaded)) << "size " << size;
        EXPECT_EQ(loaded.size(), 1u) << "size " << size;
    }
}

TEST_F(LargeDataTest, UpdateScoreInFile) {
    std::vector<Student> students = Generate(5000, 13);
    WriteFile(Write(students));

    std::mt19937 gen(99);
    std::vector<std::size_t> indices = {0, students.size() - 1};
    for (int i = 0; i < 50; ++i) {
        indices.push_back(gen() % students.size());
    }
    {
        std::fstream file = OpenFile();
        ASSERT_TRUE(file.is_open());
        for (std::size_t index : indices) {
            const auto score = static_cast<std::int32_t>(gen());
            students[index].score = score;
            EXPECT_TRUE(UpdateScore(file, students[index].id, score)) << "index " << index;
        }
        EXPECT_FALSE(UpdateScore(file, 1, 0));  // ids are multiples of an odd number
    }
    EXPECT_EQ(ReadFile(), Write(students));
}

TEST_F(LargeDataTest, UpdateScoreInStringStream) {
    std::vector<Student> students = Generate(5000, 14);
    std::stringstream ss = MakeStream(Write(students));

    std::mt19937 gen(100);
    for (int i = 0; i < 50; ++i) {
        const std::size_t index = gen() % students.size();
        const auto score = static_cast<std::int32_t>(gen());
        students[index].score = score;
        EXPECT_TRUE(UpdateScore(ss, students[index].id, score)) << "index " << index;
    }
    EXPECT_EQ(ss.str(), Write(students));
}
