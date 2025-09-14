#include <stdio.h>
#include <string.h>

#define MAX_CANDIDATE 20
#define MAX_VOTE 999

int check_winner(int vote_count[], int total_votes, int active_candidates[], int total_candidate) {
    int half_votes = total_votes / 2;
    
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i] && vote_count[i] > half_votes) {
            return i; 
        }
    }
    return -1; 
}

int find_candidate_to_eliminate(int vote_count[], int active_candidates[], int total_candidate) {
    int min_votes = MAX_VOTE + 1;
    int candidate_to_eliminate = -1;
    
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i] && vote_count[i] < min_votes) {
            min_votes = vote_count[i];
            candidate_to_eliminate = i;
        }
    }
    
    return candidate_to_eliminate;
}

void redistribute(int votes[][MAX_CANDIDATE], int vote_count[], int active_candidates[], 
                    int eliminated_candidate, int total_votes, int total_candidate) {
    
    for (int ballot = 0; ballot < total_votes; ballot++) {
        
        // Tìm lựa chọn hiện tại của từng phiếu (trong số người còn lại)
        int current_choice = -1;
        int best_preference = total_candidate + 1;
        
        for (int candidate = 0; candidate < total_candidate; candidate++) {
            if (active_candidates[candidate] && votes[ballot][candidate] < best_preference) {
                best_preference = votes[ballot][candidate];
                current_choice = candidate;
            }
        }
        
        // Nếu lựa chọn hiện tại là người bị loại
        if (current_choice == eliminated_candidate) {
            vote_count[eliminated_candidate]--;
            
            // Loại người này khỏi active (tạm thời)
            active_candidates[eliminated_candidate] = 0;
            
            // Tìm lựa chọn mới
            int new_choice = -1;
            int new_best = total_candidate + 1;
            
            for (int candidate = 0; candidate < total_candidate; candidate++) {
                if (active_candidates[candidate] && 
                    votes[ballot][candidate] < new_best) {
                    new_best = votes[ballot][candidate];
                    new_choice = candidate;
                }
            }
            
            if (new_choice != -1) {
                vote_count[new_choice]++;
            }
            
            // Khôi phục active (để xử lý phiếu tiếp theo)
            active_candidates[eliminated_candidate] = 1;
        }
    }
}
/* Kém tối ưu hơn O(v*c^2), đếm lại phiếu bầu
void redistribute(int votes[][MAX_CANDIDATE], int vote_count[], int active_candidates[], 
                 int eliminated_candidate, int total_votes, int total_candidate) {
    
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i]) {
            vote_count[i] = 0;
        }
    }
    
    // Đếm lại phiếu cho tất cả các phiếu bầu
    for (int v = 0; v < total_votes; v++) {
        // Tìm ứng viên có thứ tự ưu tiên cao nhất trong phiếu này mà chưa bị loại
        for (int preference = 1; preference <= total_candidate; preference++) {
            for (int candidate = 0; candidate < total_candidate; candidate++) {
                if (votes[v][candidate] == preference && active_candidates[candidate]) {
                    vote_count[candidate]++;
                    goto next_v; // Chuyển sang phiếu tiếp theo
                }
            }
        }
        next_v:;
    }
}*/

void sort_active_candidates(int sorted_indices[], int active_candidates[], int total_candidate, 
                           int vote_count[], char candidate[][20]) {
    int count = 0;
    
    // Lấy danh sách các ứng viên còn hoạt động
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i]) {
            sorted_indices[count++] = i;
        }
    }
    
    // Insertion sort: sắp xếp theo số phiếu giảm dần, nếu bằng thì theo alphabet
    for (int i = 1; i < count; i++) {
        int key = sorted_indices[i];
        int j = i - 1;
        
        // Di chuyển các phần tử lớn hơn key về phía sau
        while (j >= 0) {
            int current_idx = sorted_indices[j];
            
            // Kiểm tra điều kiện sắp xếp:
            // 1. Số phiếu của current_idx < số phiếu của key (cần di chuyển vì muốn giảm dần)
            // 2. Hoặc số phiếu bằng nhau nhưng tên current_idx > tên key (cần di chuyển vì muốn alphabet)
            if (vote_count[current_idx] < vote_count[key] || 
                (vote_count[current_idx] == vote_count[key] && 
                 strcmp(candidate[current_idx], candidate[key]) > 0)) {
                sorted_indices[j + 1] = sorted_indices[j];
                j--;
            } else {
                break; // Đã tìm được vị trí phù hợp
            }
        }
        
        // Chèn key vào vị trí đúng
        sorted_indices[j + 1] = key;
    }
}

int stage2(int votes[][MAX_CANDIDATE], char candidate[][20], int total_candidate, int total_votes) {
    int vote_count[MAX_CANDIDATE] = {0};
    int active_candidates[MAX_CANDIDATE];
    
    // Khởi tạo các ứng viên còn hoạt động
    for (int i = 0; i < total_candidate; i++) {
        active_candidates[i] = 1;
    }
    
    for (int v = 0; v < total_votes; v++) {
        for (int c = 0; c < total_candidate; c++) {
            if (votes[v][c] == 1) {
                vote_count[c]++;
                break;
            }
        }
    }
    
    int round = 1;
    while (1) {
        printf("round %d:\n", round);
        
        for (int i = 0; i < total_candidate; i++) {
            if (active_candidates[i]) {
                double vote_ratio = 100.0*vote_count[i]/total_candidate;
                printf("    %-20s:   %d votes, %5.1lf%%\n", candidate[i], vote_count[i], vote_ratio);
            }
        }
        
        // Kiểm tra người thắng cử
        int winner = check_winner(vote_count, total_votes, active_candidates, total_candidate);
        if (winner != -1) {
            printf("%s is declared elected\n", candidate[winner]);
            return winner;
        }
        
        // Tìm ứng viên có số phiếu thấp nhất để loại bỏ
        int eliminated = find_candidate_to_eliminate(vote_count, active_candidates, total_candidate);
        
        printf("%s is eliminated\n", candidate[eliminated]);
        
        // Phân phối lại phiếu
        redistribute(votes, vote_count, active_candidates, eliminated, total_votes, total_candidate);

        // Loại bỏ ứng viên
        active_candidates[eliminated] = 0;
        
        round++;
    }
}

int stage3(int votes[][MAX_CANDIDATE], char candidate[][20], int total_candidate, int total_votes) {
    int vote_count[MAX_CANDIDATE] = {0};
    int active_candidates[MAX_CANDIDATE];
    
    // Khởi tạo các ứng viên còn hoạt động
    for (int i = 0; i < total_candidate; i++) {
        active_candidates[i] = 1;
    }
    
    // Đếm phiếu ban đầu (ưu tiên 1)
    for (int v = 0; v < total_votes; v++) {
        for (int c = 0; c < total_candidate; c++) {
            if (votes[v][c] == 1) {
                vote_count[c]++;
                break;
            }
        }
    }
    
    int round = 1;
    while (1) {
        printf("round %d:\n", round);
        
        // Tạo mảng chỉ số để sắp xếp
        int sorted_indices[MAX_CANDIDATE];
        sort_active_candidates(sorted_indices, active_candidates, total_candidate, vote_count, candidate);
        
        // In kết quả theo thứ tự đã sắp xếp
        for (int i = 0; i < total_candidate; i++) {
            if (active_candidates[sorted_indices[i]]) {
                int idx = sorted_indices[i];
                double vote_ratio = 100.0 * vote_count[idx] / total_votes;
                printf("    %-20s:   %d votes, %5.1lf%%\n", candidate[idx], vote_count[idx], vote_ratio);
            }
        }
        
        // Kiểm tra người thắng cử
        int winner = check_winner(vote_count, total_votes, active_candidates, total_candidate);
        if (winner != -1) {
            printf("%s is declared elected\n", candidate[winner]);
            return winner;
        }
        
        // Tìm ứng viên có số phiếu thấp nhất để loại bỏ
        int eliminated = find_candidate_to_eliminate(vote_count, active_candidates, total_candidate);
        
        printf("%s is eliminated\n", candidate[eliminated]);
        
        // Phân phối lại phiếu
        redistribute(votes, vote_count, active_candidates, eliminated, total_votes, total_candidate);

        // Loại bỏ ứng viên
        active_candidates[eliminated] = 0;
        
        round++;
    }
}

// Hàm main để test
int main(int argc, char *argv[]) {
    int total_candidate = 5;
    char candidate[MAX_CANDIDATE][20] = {"Ann", "Bob", "Carl", "Dave", "Eli"};
    int votes[MAX_VOTE][MAX_CANDIDATE] = {
        {1, 2, 4, 3, 5},  
        {1, 4, 2, 3, 5},  
        {2, 5, 4, 3, 1},  
        {2, 1, 3, 4, 5}   
    };
    int total_votes = 4;
    
    int winner = stage2(votes, candidate, total_candidate, total_votes);
    int winner = stage3(votes, candidate, total_candidate, total_votes);
    
    return 0;
}
