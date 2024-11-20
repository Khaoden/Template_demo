#include <iostream>
#include <vector>
#include <initializer_list>
#include <iomanip>
#include <typeinfo>
#include <string>
#include <type_traits>

using namespace std;

// 类型萃取：检查是否为数值类型
template<typename T>
struct is_numeric_type {
    static constexpr bool value = 
        is_arithmetic<T>::value && !is_same<T, bool>::value;
};

// 矩阵类模板
template <typename T, typename = enable_if_t<is_numeric_type<T>::value>>
class Matrix {
private:
    int rows, cols;
    vector<T> data;

public:
    // 构造函数
    Matrix(int r, int c) : rows(r), cols(c) {
        data.resize(r * c);
    }

    // 使用初始化列表构造
    Matrix(initializer_list<initializer_list<T>> init) {
        rows = init.size();
        cols = init.begin()->size();
        data.resize(rows * cols);
        
        int i = 0;
        for (const auto& row : init) {
            if (row.size() != cols) {
                throw invalid_argument("所有的行必须有相同的列数");
            }
            copy(row.begin(), row.end(), data.begin() + i * cols);
            i++;
        }
    }

    // 获取元素
    T& at(int i, int j) {
        if (i < 0 || i >= rows || j < 0 || j >= cols) {
            throw out_of_range("索引越界");
        }
        return data[i * cols + j];
    }

    const T& at(int i, int j) const {
        if (i < 0 || i >= rows || j < 0 || j >= cols) {
            throw out_of_range("索引越界");
        }
        return data[i * cols + j];
    }

    // 基本运算符重载
    Matrix operator+(const Matrix& m) const {
        if (rows != m.rows || cols != m.cols) {
            throw invalid_argument("矩阵的行数和列数必须相同");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] + m.data[i];
        }
        return result;
    }

    Matrix operator-(const Matrix& m) const {
        if (rows != m.rows || cols != m.cols) {
            throw invalid_argument("矩阵的行数和列数必须相同");
        }
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] - m.data[i];
        }
        return result;
    }

    Matrix operator*(const Matrix& m) const {
        if (cols != m.rows) {
            throw invalid_argument("矩阵的行数和列数必须相同");
        }
        Matrix result(rows, m.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < m.cols; j++) {
                T sum = T();
                for (int k = 0; k < cols; k++) {
                    sum += at(i, k) * m.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

    Matrix operator*(const T& scalar) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; i++) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }

    bool operator==(const Matrix& m) const {
        if (rows != m.rows || cols != m.cols) return false;
        return data == m.data;
    }

    bool operator!=(const Matrix& m) const {
        return !(*this == m);
    }

    // 友元函数声明
    template <typename U>
    friend ostream& operator<<(ostream& os, const Matrix<U>& m);

    template <typename U>
    friend istream& operator>>(istream& is, Matrix<U>& m);

    // 获取矩阵的类型名称
    static string getTypeName() {
        if (is_same<T, int>::value) return "整数";
        if (is_same<T, float>::value) return "浮点数";
        if (is_same<T, double>::value) return "双精度浮点数";
        if (is_same<T, long>::value) return "长整数";
        if (is_same<T, short>::value) return "短整数";
        return "数值";
    }
};

// 输出运算符重载
template <typename T>
ostream& operator<<(ostream& os, const Matrix<T>& m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            os << setw(8) << m.at(i, j);
        }
        os << endl;
    }
    return os;
}

// 输入运算符重载
template <typename T>
istream& operator>>(istream& is, Matrix<T>& m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            is >> m.at(i, j);
        }
    }
    return is;
}

// 辅助打印函数
template <typename T>
void printMatrix(const Matrix<T>& m, const string& name = "") {
    if (!name.empty()) {
        cout << name << ":" << endl;
    }
    cout << m << endl;
}

// 模板函数：创建并输入矩阵
template<typename T>
Matrix<T> createAndInputMatrix(int rows, int cols) {
    Matrix<T> matrix(rows, cols);
    cout << "请输入 " << rows << "x" << cols << " 矩阵的元素：" << endl;
    cin >> matrix;
    return matrix;
}

// 模板函数：执行矩阵运算测试
template<typename T>
void testMatrixOperations() {
    cout << "\n=== 测试 " << typeid(T).name() << " 类型矩阵 ===" << endl;
    
    int rows, cols;
    cout << "请输入矩阵的行数和列数：";
    cin >> rows >> cols;

    cout << "\n输入第一个矩阵：" << endl;
    Matrix<T> m1 = createAndInputMatrix<T>(rows, cols);
    
    cout << "\n输入第二个矩阵：" << endl;
    Matrix<T> m2 = createAndInputMatrix<T>(rows, cols);

    cout << "\n=== 运算结果 ===" << endl;
    cout << "矩阵 1:" << endl << m1 << endl;
    cout << "矩阵 2:" << endl << m2 << endl;

    try {
        cout << "矩阵加法 (m1 + m2):" << endl << (m1 + m2) << endl;
        cout << "矩阵减法 (m1 - m2):" << endl << (m1 - m2) << endl;
        cout << "矩阵乘法 (m1 * m2):" << endl << (m1 * m2) << endl;
        
        T scalar;
        cout << "请输入标量值进行乘法运算：";
        cin >> scalar;
        cout << "标量乘法 (m1 * " << scalar << "):" << endl << (m1 * scalar) << endl;

        cout << "矩阵比较 (m1 == m2): " << (m1 == m2) << endl;
        cout << "矩阵比较 (m1 != m2): " << (m1 != m2) << endl;
    }
    catch (const exception& e) {
        cout << "错误：" << e.what() << endl;
    }
}

// 通用矩阵测试函数
template<typename... Types>
void runMatrixTests() {
    while (true) {
        cout << "\n=== 矩阵计算器 ===" << endl;
        
        // 动态生成类型选项
        int option = 1;
        (void)initializer_list<int>{
            (cout << option++ << ". " 
                  << Matrix<Types>::getTypeName() 
                  << " 矩阵 (" << typeid(Types).name() << ")" << endl, 0)...
        };
        cout << option << ". 退出" << endl;
        cout << "请选择数据类型 (1-" << option << "): ";

        int choice;
        cin >> choice;

        if (choice == option) {
            cout << "程序结束" << endl;
            return;
        }

        // 使用折叠表达式处理类型选择
        bool handled = false;
        int typeIndex = 1;
        (void)initializer_list<int>{
            (choice == typeIndex++ ? 
                (testMatrixOperations<Types>(), handled = true, 0) : 0)...
        };

        if (!handled) {
            cout << "无效选择，请重试" << endl;
        }
    }
}

int main() {
    // 支持的数值类型列表
    runMatrixTests<int, float, double, long, short>();
    return 0;
}