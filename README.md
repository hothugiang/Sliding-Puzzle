# Sliding-Puzzle
**I. HƯỚNG DẪN CÀI ĐẶT**
1. Tải và cài đặt mingw (nếu chưa có)
- Tải mingw qua link https://sourceforge.net/projects/mingw/
- Click đúp chuột vào file .exe vừa tải => Install
- Link hướng dẫn cài đặt: https://docs.google.com/document/d/1upNun0OxKlyYEEdvZxQ8Xc1X7DFBFwkzY4D7m8XuDNk/edit?usp=sharing

2. Tải và chạy game

Download code từ github: https://github.com/hothugiang/Sliding-Puzzle
  + Ấn nút code màu xanh bên phải màn hình
  + Ấn Download ZIP
  + Giải nén
  + Vào folder bin/Debug
  + Chạy file "siliding puzzle.exe"
  + Chơi game :>>>


**II. MÔ TẢ CHUNG VỀ TRÒ CHƠI, Ý TƯỞNG CHÍNH**

**1. Mô tả trò chơi**
- Sliding Puzzle là một tựa game xếp hình quen thuộc. Nhiệm vụ của người chơi là di chuyển các khối hình cạnh ô trống theo chiều lên, xuống hoặc trái, phải để giải ra một bức hình hoàn chỉnh
- Video Demo: https://youtu.be/94StW7wt-cA
- Cách chơi
  + Chọn kích thước game: 3x3, 4x4, 5x5
  + Góc trên phía bên phải là ảnh gốc sau khi hoàn thành, di chuyển các khối về lại vị trí đúng bằng cách bấm vào ô muốn di chuyển bên cạnh ô đen
  + Quay lại màn hình chọn độ khó ấn nút "Menu"
  + Thoát game ấn "Exit"

**2. Ý tưởng chỉnh**
- Chọn kích thước bảng (độ khó)
- Đồng hồ đếm ngược tính thời gian giải
- Bảng sắp xếp theo thứ tự sai => Giải


**III. CÁC CHỨC NĂNG ĐÃ CÀI ĐẶT**
- Bảng menu chọn kích thước
- Random ra bảng sắp xếp thứ tự sai để giải
- Các ô vuông di chuyển lên, xuống, trái, phải
- Hiệu ứng khi chọn các ô và nhạc nền


**IV. CÁC KỸ THUẬT LẬP TRÌNH ĐÃ SỬ DỤNG**
- Đặt tên biến, comment mỗi đầu đoạn code
- Thuật toán sinh ra bảng theo thứ tự không đúng để giải
- Sử dụng vector thay cho mảng bình thường
- Sử dụng SDL để chèn ảnh, chèn nhạc, chèn hiệu ứng âm thanh, chuyển động trong game


**V. KẾT LUẬN, HƯỚNG PHÁT TRIỂN, ĐIỀU RÚT RA ĐƯỢC**
- Nên chia nhỏ file để dễ quản lý
- Comment vào từng đoạn code để dễ đọc lại
- Học được thêm về 4 thư viện SDL, cách quản lý file, đặt tên biến để dễ quản lý, kĩ năng tra google, tự học,...
- Học thêm một số hàm, thư viện chưa gặp trong các bài tập
- Hướng phát triển:
  + Tính điểm, thêm rank, thêm hạng, lưu lại kết quả cao nhất của người chơi ở từng độ khó
  + Tính số bước tối ưu để giải, đếm số bước của người chơi => so sánh và tính sao
  + Thay đồng hồ đếm xuôi thành đếm ngược để thử thách
  + Setting thay đổi màu giao diện người chơi
  + Có thể thêm nút gợi ý hướng đi tiếp theo cho người chơi
  + ...
