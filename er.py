import os
import cv2
import numpy as np

# ���������ά��Ĵ�С
output_size = (45, 45)

def extract_and_resize_qr(image_path, output_path):
    # ��ȡͼ��
    img = cv2.imread(image_path)

    # ��ʼ����ά������
    qr_detector = cv2.QRCodeDetector()

    # ���ͽ����ά��
    data, bbox, _ = qr_detector.detectAndDecode(img)

    # ����ҵ���ά��
    if bbox is not None and data:
        # ��ȡ��ά��ı߽��,��ת��Ϊ�������͵�����
        bbox = bbox[0].astype(int)
        x_min = min(bbox[:, 0])
        x_max = max(bbox[:, 0])
        y_min = min(bbox[:, 1])
        y_max = max(bbox[:, 1])

        # �ü�����ά������
        qr_img = img[y_min:y_max, x_min:x_max]

        # ���ü���Ķ�ά�����Ϊָ����С
        resized_qr_img = cv2.resize(qr_img, output_size, interpolation=cv2.INTER_NEAREST)

        # ת��Ϊ�Ҷ�ͼ
        gray_qr_img = cv2.cvtColor(resized_qr_img, cv2.COLOR_BGR2GRAY)

        # ������ֵ(�����������ֵ����Ӧ��Ķ�ά��)
        threshold = 120

        # ��������ֵ��������Ϊ��ɫ,������Ϊ��ɫ
        processed_qr_img = np.where(gray_qr_img > threshold, 255, 0).astype(np.uint8)

        # ��ת��ɫ:�ڱ��,�ױ��
        inverted_qr_img = cv2.bitwise_not(processed_qr_img)

        # ����Ϊָ���ļ�·��
        cv2.imwrite(output_path, inverted_qr_img)
        print(f"QR code extracted and saved to {output_path}")
    else:
        print(f"No QR code found in {image_path}")

def process_images_in_directory(directory):
    for filename in os.listdir(directory):
        # ȷ���������ͼƬ�ļ�
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
            image_path = os.path.join(directory, filename)
            output_filename = os.path.splitext(filename)[0] + '_qr_extracted.bmp'
            output_path = os.path.join(directory, output_filename)

            # ��ȡ�͵�����ά��
            extract_and_resize_qr(image_path, output_path)

if __name__ == "__main__":
    # ��ǰ�ļ���·��
    current_directory = os.getcwd()

    # ��ʼ����ͼƬ
    process_images_in_directory(current_directory)
