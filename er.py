import os
import cv2
import numpy as np

# 设置输出二维码的大小
output_size = (45, 45)

def extract_and_resize_qr(image_path, output_path):
    # 读取图像
    img = cv2.imread(image_path)

    # 初始化二维码检测器
    qr_detector = cv2.QRCodeDetector()

    # 检测和解码二维码
    data, bbox, _ = qr_detector.detectAndDecode(img)

    # 如果找到二维码
    if bbox is not None and data:
        # 获取二维码的边界框,并转换为整数类型的坐标
        bbox = bbox[0].astype(int)
        x_min = min(bbox[:, 0])
        x_max = max(bbox[:, 0])
        y_min = min(bbox[:, 1])
        y_max = max(bbox[:, 1])

        # 裁剪出二维码区域
        qr_img = img[y_min:y_max, x_min:x_max]

        # 将裁剪后的二维码调整为指定大小
        resized_qr_img = cv2.resize(qr_img, output_size, interpolation=cv2.INTER_NEAREST)

        # 转换为灰度图
        gray_qr_img = cv2.cvtColor(resized_qr_img, cv2.COLOR_BGR2GRAY)

        # 设置阈值(调整这里的阈值以适应你的二维码)
        threshold = 120

        # 将大于阈值的像素设为白色,其他设为黑色
        processed_qr_img = np.where(gray_qr_img > threshold, 255, 0).astype(np.uint8)

        # 反转颜色:黑变白,白变黑
        inverted_qr_img = cv2.bitwise_not(processed_qr_img)

        # 保存为指定文件路径
        cv2.imwrite(output_path, inverted_qr_img)
        print(f"QR code extracted and saved to {output_path}")
    else:
        print(f"No QR code found in {image_path}")

def process_images_in_directory(directory):
    for filename in os.listdir(directory):
        # 确保处理的是图片文件
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
            image_path = os.path.join(directory, filename)
            output_filename = os.path.splitext(filename)[0] + '_qr_extracted.bmp'
            output_path = os.path.join(directory, output_filename)

            # 提取和调整二维码
            extract_and_resize_qr(image_path, output_path)

if __name__ == "__main__":
    # 当前文件夹路径
    current_directory = os.getcwd()

    # 开始处理图片
    process_images_in_directory(current_directory)
