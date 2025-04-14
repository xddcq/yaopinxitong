import os
import pandas as pd
from datetime import datetime

def txt_to_excel(data_dir, output_file):
    # 读取药品名单
    list_file = os.path.join(data_dir, "药品名单.txt")
    if not os.path.exists(list_file):
        print(f"错误：找不到药品名单文件 {list_file}")
        return False
    
    with open(list_file, 'r', encoding='utf-8') as f:
        medicine_names = [line.strip() for line in f if line.strip()]
    
    # 准备数据容器
    medicines_data = []
    
    # 药品数据目录
    medicine_data_dir = os.path.join(data_dir, "药品名单")
    
    # 处理每个药品文件
    for name in medicine_names:
        medicine_file = os.path.join(medicine_data_dir, f"{name}.txt")
        if not os.path.exists(medicine_file):
            print(f"警告：找不到药品数据文件 {medicine_file}")
            continue
        
        with open(medicine_file, 'r', encoding='utf-8') as f:
            lines = [line.strip() for line in f]
        
        # 解析药品数据
        try:
            medicine = {
                "药品名称": lines[0] if len(lines) > 0 else "",
                "药品ID": lines[1] if len(lines) > 1 else "",
                "价格": float(lines[2]) if len(lines) > 2 and lines[2] else 0.0,
                "保质期天数": int(lines[3]) if len(lines) > 3 and lines[3] else 0,
                "药品类别": lines[4] if len(lines) > 4 else "",
                "处方药": "是" if len(lines) > 5 and lines[5] == "1" else "否",
                "库存数量": int(lines[6]) if len(lines) > 6 and lines[6] else 0,
                "最后入库日期": lines[7] if len(lines) > 7 else "",
                "生产日期": "",
                "过期日期": ""
            }
            
            # 计算生产日期和过期日期（如果有最后入库日期和保质期天数）
            if medicine["最后入库日期"] and medicine["保质期天数"] > 0:
                try:
                    last_stock_date = datetime.strptime(medicine["最后入库日期"], "%Y-%m-%d")
                    production_date = last_stock_date - pd.Timedelta(days=medicine["保质期天数"])
                    medicine["生产日期"] = production_date.strftime("%Y-%m-%d")
                    medicine["过期日期"] = medicine["最后入库日期"]
                except ValueError:
                    pass
            
            medicines_data.append(medicine)
        except Exception as e:
            print(f"解析药品 {name} 数据时出错: {str(e)}")
            continue
    df = pd.DataFrame(medicines_data)
    columns = [
        "药品ID", "药品名称", "药品类别", "处方药", 
        "价格", "库存数量", "保质期天数",
        "生产日期", "过期日期", "最后入库日期"
    ]
    df = df[[col for col in columns if col in df.columns]]
    
    # 保存为Excel
    try:
        df.to_excel(output_file, index=False, engine='openpyxl')
        print(f"成功导出 {len(df)} 条药品数据到 {output_file}")
        return True
    except Exception as e:
        print(f"导出Excel失败: {str(e)}")
        return False

if __name__ == "__main__":
    data_directory = "数据保存"  
    output_excel = "药品库存数据.xlsx"  # 输出的Excel文件名
    
    if txt_to_excel(data_directory, output_excel):
        print("导出完成！")
    else:
        print("导出过程中出现错误。")