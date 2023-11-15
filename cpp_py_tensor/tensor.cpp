#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>

/**
 * @brief:  check and gen tensor;
 * @author: PeppermintSummer;
*/

template<typename T_>
struct Matrix{
    std::vector<T_> data_;
    std::vector<int> shape_;
};

template<typename T_>
bool save_tensor_to_file(const std::string& file, std::shared_ptr<Matrix<T_>> matrix){
    // assert matrix is not nullptr!
    if(matrix == nullptr){
        return false;
    }
    FILE* f = fopen(file.c_str(),"wb");
    if(f == nullptr) return false;
    std::vector<int> shape_ = matrix->shape_;
    uint ndims = shape_.size();
    uint head[3] = {0xFCCFE2E2, ndims, static_cast<uint>(1)}; // enum dtype
    fwrite(head,1,sizeof(head),f); // elem_ptr, elem_count,per_elem_size ,file_ptr
    fwrite(shape_.data(),1,sizeof(shape_[0])*shape_.size(),f);
    fwrite(matrix->data_.data(),1,sizeof(T_) * matrix->data_.size(),f);
    fclose(f);
    return true;
}

template<typename T_>
bool load_tensor_from_file(const std::string& file, std::shared_ptr<Matrix<T_>> matrix){
    FILE* f = fopen(file.c_str(), "rb");
    if(f == nullptr) {
        printf("file open failed");
        return false;
    }
    uint head[3] = {0};
    fread(head,1,sizeof(head),f);
    if(head[0] != 0xFCCFE2E2){
        fclose(f);
        printf("%s is not your tensor\n",file.c_str());
        return false;
    }
    int ndims = head[1]; // shape size; shape -> elem_size
    int dtype = head[2];
    printf("%d\n",ndims);
    std::vector<int> dims(ndims);
    fread(dims.data(),1,ndims * sizeof(dims[0]),f);
    matrix->shape_ = dims;
    uint elem_numel = std::accumulate(dims.begin(),dims.end(),1,std::multiplies<T_>());;
    matrix->data_.resize(elem_numel);
    fread(matrix->data_.data(),1,sizeof(T_) * matrix->data_.size(),f);
    fclose(f);
    return true;
}

template<typename T>
void show_matrix(const Matrix<T> matrix){
    printf("========data=========\n");
    for(int i  =0 ;i<matrix.data_.size();i++){
        printf("%d ",matrix.data_.at(i));
    }
    printf("\n========shape=========\n");
    for(int i  =0 ;i<matrix.shape_.size();i++){
        printf("%d ",matrix.shape_.at(i));
    }
    printf("\n");
}

int main(){
    auto matrix_ptr = std::make_shared<Matrix<int>>();
    // matrix_ptr->data_ = {1,2,3};
    // matrix_ptr->shape_ = {1,3};
    // save_tensor_to_file("tensor.bin",matrix_ptr);
    load_tensor_from_file("tensor.bin",matrix_ptr);

    show_matrix(*matrix_ptr);
}